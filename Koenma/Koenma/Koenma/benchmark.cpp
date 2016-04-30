/*
Koenma, a UCI chess playing engine derived from Stockfish 7
Copyright (C) 2016-2017 grefen

Koenma is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Koenma is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <fstream>
#include <iostream>
#include <istream>
#include <vector>

#include "misc.h"
#include "position.h"
#include "search.h"
#include "thread.h"
#include "uci.h"

using namespace std;

namespace {

	const vector<string> Defaults = {
	    "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1",
	    "r1bakabnr/9/1cn4c1/p1p1p1p1p/9/9/P1P1P1P1P/6CC1/9/RNBAKABNR w - - 0 1",
	    "2b1ka3/4a4/4b4/9/9/9/4r4/5K3/9/9 w - - 0 1",
	    "2baka3/9/9/9/6b2/9/5r3/9/5K3/9 w - - 0 1",

		"3ak1b2/4a4/b5n1c/p5C1p/4p1P2/1R7/P4r2P/4B1NrC/2nRA4/4KAB2 w - - 0 21",
		"4k4/5c3/5a2b/3N4p/6b2/P1B6/8P/B2A5/3n5/3A1K3 w - - 4 2",
		"Cnbakabn1/9/1c2r4/p1p3p1p/9/6c2/P1P1P3P/4C4/9/RNBAKABNR b - - 0 7",
		"Cn2kabnr/4a4/7c1/p1p3p1p/4p4/9/P1P1P1P1P/1C7/9/RcBAKABNR b - - 0 4",
		"rnbakabnr/9/4c4/p3p1C1p/9/9/P1P1c1P1P/1C7/9/RNBAKABNR b - - 0 4",
		"r1bakabnr/9/2n4c1/p1p3p1p/4C4/9/P1P1c1P1P/6C2/9/RNBAKABNR b - - 3 4",
		"rnCakabnr/9/2c6/p5p1p/4C4/9/c1P3P1P/N8/9/R1BAKABNR b - - 0 6",
		"rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1",
		//"3ak4/2N5r/3N4r/9/9/9/9/9/9/4K4 b - - 0 1",//king face to face error position
		"3ak4/9/9/r8/4R4/4C4/9/9/9/4K4 b - - 0 1",
		"4k4/2P6/4R4/9/9/9/9/9/9/4K4 b - - 0 1",
		"4k4/9/4R4/4C4/9/9/9/9/9/4K4 b - - 0 1",
		"4k4/4C4/4C4/9/9/9/9/9/9/4K4 b - - 0 1",
		"4k4/2N6/2rr5/4p4/9/9/9/9/9/4K4 b - - 0 1",
		"4k4/4P4/9/9/9/9/9/9/9/4K4 b - - 0 1",
		"3ak4/9/9/4C4/9/3p5/2n6/3pC4/9/4K4 b - - 0 1",
		"4k4/9/c8/4C4/9/c8/9/4C4/9/4K4 b - - 0 1",
		"4k4/9/9/4C4/9/c8/9/4C4/9/4K4 b - - 0 1",
		"4k4/9/9/4C4/9/9/9/4C4/9/4K4 b - - 0 1",
		"9/2N1k4/9/9/4N4/9/9/9/9/c1B1K4 w - - 0 1",
		"9/4k4/5N3/3N5/9/9/4c4/4B4/9/3K5 b - - 8 161",
		"9/2Nck4/9/9/4N4/9/9/4B4/9/3K5 w - - 9 161",
		"9/2N1k4/4cN3/9/9/9/9/4B4/9/4K4 w - - 4 15",
		"9/4k4/3c1N3/3N5/9/9/9/4B4/9/3K5 w - - 9 156",
		"4k4/2N6/3c1N3/9/9/9/9/4B4/9/4K4 b - - 0 1",//
		"3R5/4k4/9/9/9/9/9/9/9/3K5 w - - 0 1",//
		"3Rk4/9/9/9/9/9/9/9/9/3K5 b - - 0 1",//
		"3ak4/9/2n2c3/9/4R4/9/9/4B4/9/4K4 b - - 0 1",//
		"4k4/9/9/9/4R4/9/9/4B4/9/4K4 b - - 0 1",//
		"4k4/9/9/4p4/4C4/9/9/4B4/9/4K4 b - - 0 1",//
		"4k4/9/9/9/9/9/9/4N4/9/4K4 w - - 0 1",
		"4k4/9/9/9/9/9/9/4B4/9/4K4 w - - 0 1",//
		"4k4/9/9/9/9/9/9/9/4A4/4K4 w - - 0 1",//
		"4k4/9/3NP4/9/9/9/9/9/9/4K4 b - - 0 1",//
		"4k4/9/4P4/9/9/9/9/9/9/4K4 w - - 0 1",
		"4k4/9/9/4R4/9/9/9/9/9/4K4 b - - 0 1",
		"rnbakabnr/4c4/4c4/2p1C1p1p/p8/9/P1P1P1P1P/7C1/9/RNBAKABNR w - - 0 5",
		"rnbakabnr/4C4/4c4/2p3p1p/p8/9/P1P1P1P1P/7C1/9/RNBAKABNR w - - 0 5",
		"rnbakabnr/4c4/7c1/2p1C1p1p/p8/9/P1P1P1P1P/7C1/9/RNBAKABNR b - - 1 4",
		"rnbakabnr/9/1c5c1/2p1p1p1p/p8/9/P1P1P1P1P/3C3C1/9/RNBAKABNR w - - 1 2",
		"rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1"
	};

} // namespace

/// benchmark() runs a simple benchmark by letting Stockfish analyze a set
/// of positions for a given limit each. There are five parameters: the
/// transposition table size, the number of search threads that should
/// be used, the limit value spent for each position (optional, default is
/// depth 13), an optional file name where to look for positions in FEN
/// format (defaults are the positions defined above) and the type of the
/// limit value: depth (default), time in millisecs or number of nodes.

void benchmark(const Position& current, istream& is) {

	string token;
	vector<string> fens;
	Search::LimitsType limits;

	// Assign default values to missing arguments
	string ttSize = (is >> token) ? token : "16";
	string threads = (is >> token) ? token : "1";
	string limit = (is >> token) ? token : "13";
	string fenFile = (is >> token) ? token : "default";
	string limitType = (is >> token) ? token : "depth";

	Options["Hash"] = ttSize;
	Options["Threads"] = threads;
	Search::clear();

	if (limitType == "time")
		limits.movetime = stoi(limit); // movetime is in millisecs

	else if (limitType == "nodes")
		limits.nodes = stoi(limit);

	else if (limitType == "mate")
		limits.mate = stoi(limit);

	else
		limits.depth = stoi(limit);

	if (fenFile == "default")
		fens = Defaults;

	else if (fenFile == "current")
		fens.push_back(current.fen());

	else
	{
		string fen;
		ifstream file(fenFile);

		if (!file.is_open())
		{
			cerr << "Unable to open file " << fenFile << endl;
			return;
		}

		while (getline(file, fen))
			if (!fen.empty())
				fens.push_back(fen);

		file.close();
	}

	uint64_t nodes = 0;
	TimePoint elapsed = now();

	for (size_t i = 0; i < fens.size(); ++i)
	{
		Position pos(fens[i], Options["UCI_Chess960"], Threads.main());

		cerr << "\nPosition: " << i + 1 << '/' << fens.size() << endl;

		if (limitType == "perft")
			nodes += Search::perft(pos, limits.depth * ONE_PLY);

		else
		{
			Search::StateStackPtr st;
			limits.startTime = now();
			Threads.start_thinking(pos, limits, st);
			Threads.main()->wait_for_search_finished();
			nodes += Threads.nodes_searched();
		}
	}

	elapsed = now() - elapsed + 1; // Ensure positivity to avoid a 'divide by zero'

	dbg_print(); // Just before to exit

	cerr << "\n==========================="
		<< "\nTotal time (ms) : " << elapsed
		<< "\nNodes searched  : " << nodes
		<< "\nNodes/second    : " << 1000 * nodes / elapsed << endl;
}
