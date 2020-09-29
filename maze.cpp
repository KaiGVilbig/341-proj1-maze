#include <stdexcept>
#include "maze.h"

Maze::Maze(int ncell) {
  if (ncell < 1) {
    throw std::invalid_argument("Maze::Maze(): ncell must be >= 1");
  }

  _ncell = ncell;
  _maze = new cell_t[_ncell];
}

Maze::~Maze() {
  // IMPLEMENT DESTRUCTOR
	delete[] _maze;
	_maze = NULL;
}

void Maze::readFile(std::string fileName) {
  int numCells;
  int cell, n1, n2, n3, n4; 
  std::ifstream dataFile;
  
  dataFile.open(fileName);

  if (dataFile.is_open()) {
    dataFile >> numCells;
    this->reset(numCells);

    for (int i = 0; i < numCells; i++) {
      dataFile >> cell >> n1 >> n2 >> n3 >> n4;
      this->insert(cell_t(cell, n1, n2, n3, n4));
    }
  }
}

int Maze::getNcell() const {
  return _ncell;
}

void Maze::reset(int ncell) {
  if (ncell < 1) {
    throw std::invalid_argument("Maze::reset(): ncell must be >= 1");
  }

  if (_maze != nullptr) {
    delete [] _maze;
  }
  
  _ncell = ncell;
  _maze = new cell_t[_ncell];
}

void Maze::insert(cell_t cell) {
  if (_maze == nullptr) {
    throw std::domain_error("Maze::insert(): attempt to insert into uninitialized Maze object");
  }
  if (cell.cellNum < 0 || cell.cellNum >= _ncell) {
    throw std::invalid_argument("Maze:insert(): invalid cell number");
  }
  _maze[cell.cellNum] = cell;
}
  
cell_t Maze::retrieve(int nc) const {
  if (_maze == nullptr) {
    throw std::domain_error("Maze::retrieve(): attempt to retrieve from an uninitialized Maze object");
  }
  if (nc < 0 || nc >= _ncell) {
    throw std::invalid_argument("Maze:retrieve(): invalid cell index");
  }
  
  return _maze[nc];
}

void Maze::dump() const {

  std::cout << "[*] Dumping the maze cells..." << std::endl;
  
  for (int i = 0; i < _ncell; i++) {
    int nc = _maze[i].cellNum;
    if (nc != i) {
      std::cerr << "Warning: maze cell at index " << i
		<<" has cellNum of " << nc << std::endl;
    }
    std::cout << "Cell " << i << " has neighbor cells: ";
    neighbor_t nbs = _maze[i].neighbors;
    int n0, n1, n2, n3;
    if ( nbs[0] >= 0 ) std::cout << nbs[0] << " ";
    if ( nbs[1] >= 0 ) std::cout << nbs[1] << " ";
    if ( nbs[2] >= 0 ) std::cout << nbs[2] << " ";
    if ( nbs[3] >= 0 ) std::cout << nbs[3] << " ";
    std::cout << std::endl;
  }
}

std::vector<int> Maze::solve() const {
  // IMPLEMENT THE SOLVE METHOD
	std::cout << "[*] Start of call to solve()...\n";
	
	bool noSoln = false;
	int cn = _maze[0].cellNum;
	neighbor_t nbs = _maze[0].neighbors;
	int prevCn = -1;
	SolveStack solve;
	src_dest_t start(VACANT, 0);
	solve.push(start);

	neighbor_t soln;

	//keep solving until we are in cell n - 1
	while (cn != _ncell - 1 && !noSoln)
	{
		//check to see if no soln
		if (cn == 0 && nbs[0] == VACANT && nbs[1] == VACANT && nbs[2] == VACANT
			&& nbs[3] == VACANT)
		{
			std::cout << "[-] No solution found.\n";
			noSoln = true;
			break;
		}

		//check each neighbour
		for (int i = 0; i <= 3; i++)
		{
			if (nbs[i] != VACANT && nbs[i] != prevCn)
			{
				src_dest_t src(cn, nbs[i]);
				solve.push(src);
				prevCn = cn;
				cn = _maze[nbs[i]].cellNum;
				nbs = _maze[nbs[i]].neighbors;
				break;
			}
			else if (i == 3 && nbs[i] == VACANT)
			{
				int del = cn;
				src_dest_t bad;
				bad = solve.pop();
				//nbs[i] = VACANT;
				cn = _maze[bad.second].cellNum;
				nbs = _maze[bad.second].neighbors;
				prevCn = bad.first;
				for (int i = 0; i <= 3; i++)
				{
					if (nbs[i] == del)
					{
						//nbs[i] = VACANT;
						_maze[cn].neighbors[i] = VACANT;
					}
				}
				break;
			}
		}
	}
	
	//puts the stack into a vector
	while (solve.read().second != 0)
	{
		src_dest_t cn = solve.pop();
		soln.push_back(cn.second);
	}

	//since the vector is reversed, we must fix it
	neighbor_t fixedSoln;
	int x = soln.size();
	int num = x - 1;
	for (int i = 0; i < x; i++)
	{
		fixedSoln.push_back(soln[num]);
		num--;
	}

	if (!noSoln)
	{
		fixedSoln.push_back(_ncell - 1);
		std::cout << "[+] Solution found!\n";
	}

	return fixedSoln;
}

Maze::SolveStack::SolveStack() {
  _stack = nullptr;
}

Maze::SolveStack::~SolveStack() {
  // IMPLEMENT DESTRUCTOR
	while (_stack != nullptr)
	{
		StackEntry* curr = _stack;
		_stack = curr->next;
		delete curr;
		curr = nullptr;
	}
}

bool Maze::SolveStack::empty() const {
  return _stack == nullptr;
}
				   
void Maze::SolveStack::push(src_dest_t src_dest) {
  // IMPLEMENT PUSH METHOD
	StackEntry* entry = new StackEntry(src_dest);
	if (_stack != nullptr)
	{
		entry->next = _stack;
		_stack = entry;
	}
	else
	{
		entry->next = nullptr;
		_stack = entry;
	}
}

src_dest_t Maze::SolveStack::pop() {
  // IMPLEMENT POP METHOD
	src_dest_t retVal;
	if (_stack != nullptr)
	{
		StackEntry *curr = _stack;
		_stack = curr->next;
		delete curr;
		retVal = _stack->src_dest;
	}
	return retVal;
}

src_dest_t Maze::SolveStack::read() const {
  // IMPLEMENT READ METHOD
	src_dest_t readThis = _stack->src_dest;
	return readThis;
}
