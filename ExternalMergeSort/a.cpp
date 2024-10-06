#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <chrono>
using namespace std;

// The structure of the Loser Tree
class LoserTree
{
private:
    int k;
    int winner;                // Number of players (input lists)
    vector<int> tree;          // The tree that keeps track of losers
    vector<vector<int>> lists; // Input sorted lists
    vector<int> current;       // Current value for each list
    vector<int> currentIndex;

public:
    LoserTree(vector<vector<int>> &inputLists, vector<int> &currentIndex);
    void buildTree();                     // Initialize the tree
    void playMatch(int node, int player); // Simulate a match between tree nodes
    int getWinner();                      // Get the current winner (smallest value)
    void updateTree(int player);          // Update the tree after the winner is replaced
    vector<vector<int>> &getLists();
    vector<int> &getCurrentIndexes();
};

// Constructor to initialize the Loser Tree
LoserTree::LoserTree(vector<vector<int>> &inputLists, vector<int> &currentIndx)
{
    k = inputLists.size();
    tree.resize(k - 1, -1); // Initialize tree with -1 (no loser)
    lists = inputLists;     // Store the input lists
    current.resize(k);      // Store the current value of each list
    currentIndex = currentIndx;

    // Initialize current values to the first element of each list
    for (int i = 0; i < k; ++i)
    {
        if (!lists[i].empty())
        {
            current[i] = lists[i][0]; // Get the first element from each list
        }
        else
        {
            current[i] = numeric_limits<int>::max(); // Mark empty lists with max value
        }
    }

    buildTree();
}

vector<vector<int>> &LoserTree::getLists()
{
    return lists;
}
vector<int> &LoserTree::getCurrentIndexes()
{
    return currentIndex;
}
// Build the Loser Tree by simulating matches from leaves to the root
void LoserTree::buildTree()
{

    for (int i = 0; i < k; ++i)
    {
        // cout << ((k - 1 - i % 2) + i) / 2 << "-- " << i << endl;
        playMatch(((k - 1 - i % 2) + i) / 2, i);
    }
}

// Simulate a match between two players (lists) and store the loser in the tree
void LoserTree::playMatch(int node, int player)
{
    // cout << "playmatch " << node << "-- " << player << endl;
    if (player == -1)
    {
        return;
    }
    if (node == 0)
    {
        if (tree[node] == -1 && player != -1)
        {
            tree[node] = player;
        }
        else if (player == -1)
        {
            return;
        }
        else if (current[player] <= current[tree[node]])
        {

            winner = player;
            // cout << "winner is " << player << "--" << current[winner] << endl;
        }
        else
        {
            int temp = tree[node];
            tree[node] = player;
            winner = temp;
            // cout << "winner is (current[player] >current[tree[node]]) " << player << "--" << current[winner] << endl;
        }
        return; // If at the root, return
    }

    int parent = (node - 1) / 2; // Parent node
    if (tree[node] == -1 || current[player] >= current[tree[node]])
    {
        // Current player wins; loser goes to the parent node
        int temp = tree[node];
        tree[node] = player;
        playMatch(parent, temp); // Continue with the loser
    }
    else
    {
        // Current player loses; pass it to the parent
        playMatch(parent, player);
    }
}

// Get the current winner (smallest element among lists)
int LoserTree::getWinner()
{
    // return lists[winner][0]; // The root stores the index of the winner
    return winner;
}

// Update the tree after replacing the winner with the next element in its list
void LoserTree::updateTree(int player)
{

    // Move to the next element in the list for the current winner

    currentIndex[player]++;
    // cout << "current index of player " << player << "is " << currentIndex[player] << endl;
    current[player] = (currentIndex[player] < lists[player].size()) ? lists[player][currentIndex[player]] : numeric_limits<int>::max();
    // cout << "current player is " << current[player] << endl;
    playMatch((player + (k - 1 - player % 2)) / 2, player); // Rebuild the tree starting from the player
    // playMatch((player + (k - 1 - player % 2)) / 2, player);
}

int main()
{
    // Example input: sorted lists (can also be file data)
    vector<vector<int>> lists = {
        {1, 1, 1, 1, 2},
        {1, 1, 2, 3, 3},
        {1, 2, 3, 5, 9},
        {1, 1, 2, 6, 10},
        {1, 2, 3, 7, 11},
        {1, 1, 4, 8, 12},
        {10, 12, 13, 13, 14},
        {10, 12, 19, 19, 20},
        {2, 3, 4, 4, 5},
        {4, 5, 5, 5, 6},
        {1, 1, 1, 1, 2},
        {1, 2, 2, 3, 3},
        {1, 1, 2, 2, 2},
        {1, 1, 2, 3, 3},
        {2, 3, 4, 5, 5},
        {4, 5, 5, 6, 6},

    };
    vector<int> currentIndex(16, 0);
    LoserTree loserTree(lists, currentIndex);
    auto start = chrono::high_resolution_clock::now();
    cout << "Merged Sorted Output: ";
    for (int i = 0; i < 80; i++)
    {
        // cout << "----------------------" << lists[0][0] << endl;
        //  There are 12 elements in total in the example
        int winner = loserTree.getWinner();                                                 // Get the smallest element's index
        cout << loserTree.getLists()[winner][loserTree.getCurrentIndexes()[winner]] << " "; // Output the smallest element
        // cout << "Winner is " << winner << endl;
        loserTree.updateTree(winner); // Update the tree with the next element from the winner's list
    }
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Execution time: " << elapsed.count() << " seconds" << endl;

    return 0;
}
