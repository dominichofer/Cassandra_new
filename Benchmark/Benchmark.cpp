#include "BoardCollection.h"
#include "Path.h"
#include "Utility.h"
#include "PositionGenerator.h"
#include "Search.h"
#include <thread>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <iterator>

int main(int argc, char* argv[])
{
	const std::size_t SEED = 65481265;
	std::vector<std::size_t> Positions_per_EmptyCount(22);
	Positions_per_EmptyCount[ 0] = 1'000'000;
	Positions_per_EmptyCount[ 1] = 1'000'000;
	Positions_per_EmptyCount[ 2] = 1'000'000;
	Positions_per_EmptyCount[ 3] = 1'000'000;
	Positions_per_EmptyCount[ 4] = 1'000'000;
	Positions_per_EmptyCount[ 5] = 1'000'000;
	Positions_per_EmptyCount[ 6] = 1'000'000;
	Positions_per_EmptyCount[ 7] =   500'000;
	Positions_per_EmptyCount[ 8] =   250'000;
	Positions_per_EmptyCount[ 9] =   100'000;
	Positions_per_EmptyCount[10] =    50'000;
	Positions_per_EmptyCount[11] =    25'000;
	Positions_per_EmptyCount[12] =    10'000;
	Positions_per_EmptyCount[13] =     5'000;
	Positions_per_EmptyCount[14] =     2'500;
	Positions_per_EmptyCount[15] =     1'000;
	Positions_per_EmptyCount[16] =       500;
	Positions_per_EmptyCount[17] =       250;
	Positions_per_EmptyCount[18] =       100;
	Positions_per_EmptyCount[19] =        50;
	Positions_per_EmptyCount[20] =        25;
	Positions_per_EmptyCount[21] =        10;

	CPositionGenerator pos_gen(SEED);
	std::size_t threads = std::thread::hardware_concurrency();

	auto env = std::make_shared<Environment>(std::make_shared<CConfigurations>(), std::make_shared<CHashTablePVS>(16'000'000));

	for (std::size_t EmptyCount = 0; EmptyCount < Positions_per_EmptyCount.size(); EmptyCount++)
	{
		const auto PosCount = Positions_per_EmptyCount[EmptyCount];
		const auto pos_set = pos_gen.GenerateRandomPositionSet(EmptyCount, PosCount);
		std::vector<CPosition> positions(pos_set.begin(), pos_set.end());

		std::vector<std::unique_ptr<Search>> searches;
		for (std::size_t i = 0; i < positions.size(); i++)
			searches.emplace_back(std::make_unique<PVSearch>(env));

		const auto startTime = std::chrono::high_resolution_clock::now();
		#pragma omp parallel for schedule(static, 1) num_threads(threads)
		for (int64_t i = 0; i < positions.size(); i++)
		{
			const auto& pos = positions[i];
			std::unique_ptr<Search>& search = searches[i];

			search->Eval(pos);
		}
		const auto endTime = std::chrono::high_resolution_clock::now();

		std::size_t NodeCounter = 0;
		for (auto& search : searches)
			NodeCounter += search->GetNodeCount();

		const auto pico_seconds = std::chrono::duration_cast<std::chrono::duration<long long, std::pico>>(endTime - startTime);
		const auto ps_er_pos = pico_seconds / positions.size();
		std::cout 
			<< std::setw(2) << EmptyCount << " Empties: "
			<< short_time_format(ps_er_pos) << "/pos, "
			<< std::setprecision(2) << std::scientific << ps_er_pos.count() / (double)std::pico::den << " s/pos, "
			<< std::setprecision(2) << std::scientific << 1.0 / ps_er_pos.count() * std::pico::den << " pos/s" << std::endl;
	}
	return 0;
}