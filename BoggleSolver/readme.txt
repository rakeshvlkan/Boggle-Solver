This is a c++ based console application for solving Boggle Word game. The more detail about the game can be found at https://en.wikipedia.org/wiki/Boggle and also this application 
implemented based on the rules of the game mentioned at wiki link. 

If any one wants create boggle game, they can use this application as a back end code and can be integrated with your game. I have added few test cases for checking the my boggle solver 
and all tests are passed. Currently it is taking 6 seconds to solve very large size Boggle(100x100) and still I am trying reduce the excution time. Also it is taking more memory for boggle of size 200x200 and will update
the code soon.

Below are the API's can be used in the order mentioned: 

// Pass the dictionary file path as parameter
void LoadDictionary(const char * path)


// Pass Boggle board with width and height as parameter. 
// Returns results which shows found words and the score
Results FindWords(const char * board, unsigned width, unsigned height)


// Delete the results
void FreeWords(Results results)


// Delete contents from dictionary tree
void FreeDictionary()


For example: 

Input:

If you give input boggle board as: 
							   m m g
							   e u s
							   k d e

Output:
							 
Below words should be found from the boggle solver. 

   {
		"des", "due", "dues", "dug", "dugs", "duke", "ems", "emu",
		"emus", "gude", "gudes", "gum", "gummed", "gums", "kue",
		"kues", "mem", "mems", "mud", "muds", "mug", "mugs", "mum",
		"mums", "mus", "muse", "mused", "sedum", "smug", "sue",
		"sued", "suede", "sum", "summed", "uke", "use", "used"
	};
	
 
 Total words found: 37
 Total count      : 45


On Windows:

Open the solution "BoggleSolverAssignment.sln" and build it in any one of the mode.
Run the application.


On Linux:

Compile:
g++ -std=c++11 main.cpp boggleSolver.cpp -o boggleSolver


Run:
./boggleSolver


OutPut:

Displays test pass results for each type of Board and at the end displays "Success!" if all the tests are passed 
otherwise it will display test name and data mismatches if any.


Note:
This solver currently supports only lowercase letters
