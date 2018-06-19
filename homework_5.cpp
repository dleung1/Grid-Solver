//Dickens Leung
//EECE 643

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <vector>

using namespace std;

/*change variables to suit needs
    max_length = to the grid size
    input_name refers to the file being read
    output_name refers to the file being written
*/
const int max_length=6;
string input_name="input2.txt";
string output_name="output.txt";

struct cell_structure
{
    int cellCost;
    char predecessor;
    bool reached;
};

struct input_data
{
    char target;
    int x, y;
    string out;
};

int open_file(vector<input_data> &data, vector<input_data> &modified_data, int &num_of_targets);
int maze(vector<input_data> &data);
bool bounded(int x, int y);
int output_file(vector<input_data> data);

int main()
{
    vector<input_data> data, modified_data;
    string out;
    int num_of_targets=0;
    int x_coor, y_coor;

    //error if no input file
    if(open_file(data, modified_data, num_of_targets))
    {
        cout<<"Unable to open input file"<<endl;
        return 1;
    }

    //call maze for single or multiple targets
    for(int i = 0; i < num_of_targets; i++)
    {
        //error if no path possible to target
        if(maze(modified_data))
        {
            cout<<"No path to target"<<endl;
            return 2;
        }

        //read from output string from modified_data vector and add string to data vector
        //read from output string and add sources to modified_data
        for(vector<input_data>::iterator i = modified_data.begin(); i != modified_data.end(); i++)
        {
            for(vector<input_data>::iterator j = data.begin(); j != data.end(); j++)
            {

                if(i->target == 'T' && j->target == 'T' && i->x == j->x && i->y == j->y)
                {
                    j->out = i->out;
                    i->target = 'S';

                    x_coor = j->x;
                    y_coor = j->y;

                    for(int k=0; k<j->out.length() - 2; k++)
                    {
                        switch(j->out[k])
                        {
                            case 'N':   modified_data.push_back(input_data());
                                        i = modified_data.end() - 1;

                                        x_coor = x_coor - 1;
                                        i->target = 'S';
                                        i->x = x_coor;
                                        i->y = y_coor;
                                        break;
                            case 'S':   modified_data.push_back(input_data());
                                        i = modified_data.end() - 1;

                                        x_coor = x_coor + 1;
                                        i->target = 'S';
                                        i->x = x_coor;
                                        i->y = y_coor;
                                        break;
                            case 'W':   modified_data.push_back(input_data());
                                        i = modified_data.end() - 1;

                                        y_coor = y_coor - 1;
                                        i->target = 'S';
                                        i->x = x_coor;
                                        i->y = y_coor;
                                        break;
                            case 'E':   modified_data.push_back(input_data());
                                        i = modified_data.end() - 1;

                                        y_coor = y_coor + 1;
                                        i->target = 'S';
                                        i->x = x_coor;
                                        i->y = y_coor;
                                        break;
                            default: break;
                        }
                    }
                }
            }
        }
    }

    if(output_file(data))
    {
        cout<<"Unable to open output file"<<endl;
        return 3;
    }

    return 0;
}

//Opens text file and parses
//returns data
int open_file(vector<input_data> &data, vector<input_data> &modified_data, int &num_of_targets)
{
    string line;
    ifstream inFile;
    int i=0;

    inFile.open(input_name.c_str());

    //Unable to open file
    if(!inFile) return 1;

    //load data from input file to vector
    while( getline(inFile,line))
    {
        data.push_back(input_data());
        data[i].target = line[0];
        data[i].x = line[3] - 48 - 1;
        data[i].y = line[5] - 48 - 1;

        modified_data.push_back(input_data());
        modified_data[i].target = line[0];
        modified_data[i].x = line[3] - 48 - 1;
        modified_data[i].y = line[5] - 48 - 1;

        if(data[i].target == 'T') num_of_targets++;

        i++;
    }

    inFile.close();
    return 0;
}

//Outputs data to file
int output_file(vector<input_data> data)
{
    ofstream output_file(output_name.c_str());

    //Unable to open file
    if(!output_file.is_open()) return 1;

    for(vector<input_data>::iterator i = data.begin(); i != data.end(); i++)
    {
        output_file << i->target << " (" << i->x + 1 << "," << i->y + 1 << ") ";
        if(i->target == 'T') output_file << i->out;
        output_file << endl;
    }

    output_file.close();

    return 0;
}

//Maze algorithm to find path from source to target
int maze(vector<input_data> &data)
{
    cell_structure grid[max_length][max_length];
    vector<input_data> wavefront_structure;
    vector<input_data>::iterator w = wavefront_structure.begin();

    int counter=0;
    int x_counter, y_counter=0;
    char predecessor;

    //Make first wavefront equal to source or multiple sources
    for(vector<input_data>::iterator i = data.begin(); i != data.end(); i++)
    {
        if(i->target == 'S')
        {
            wavefront_structure.push_back(input_data());
            wavefront_structure[counter].target = i->target;
            wavefront_structure[counter].x = i->x;
            wavefront_structure[counter].y = i->y;
            grid[wavefront_structure[counter].x][wavefront_structure[counter].y].cellCost = 1;
            grid[wavefront_structure[counter].x][wavefront_structure[counter].y].predecessor = 'Z';
            grid[wavefront_structure[counter].x][wavefront_structure[counter].y].reached = true;

            counter++;
        }
    }

    while(1)
    {
        if(wavefront_structure.empty()) return 1;

        //check if current wavefront cell is equal to any targets
        for(vector<input_data>::iterator y = data.begin(); y != data.end(); y++)
        {
            if(wavefront_structure[0].x == y->x && wavefront_structure[0].y == y->y && y->target == 'T')
            {
                x_counter = 0;
                y_counter = 0;

                //backtrace path
                for(int i = 1; i < grid[wavefront_structure[0].x][wavefront_structure[0].y].cellCost; i++)
                {
                    y->out = y->out + grid[wavefront_structure[0].x + x_counter][wavefront_structure[0].y + y_counter].predecessor + " ";
                    switch(grid[wavefront_structure[0].x + x_counter][wavefront_structure[0].y + y_counter].predecessor)
                    {
                        case 'N':   x_counter = x_counter - 1;
                                    break;
                        case 'S':   x_counter = x_counter + 1;
                                    break;
                        case 'W':   y_counter = y_counter - 1;
                                    break;
                        case 'E':   y_counter = y_counter + 1;
                                    break;
                    }
                }
                return 0;
            }
        }

        /*  Check north, west, south, and east of current cell
            if checked cell is not reached and is within the grid then add to the end of wavefront
        */
        for(int i=0; i < 4; i++)
        {
            switch(i)
            {
                case 0: x_counter = -1; //check north
                        y_counter = 0;
                        predecessor = 'S';
                        break;
                case 1: x_counter = 1; //check south
                        y_counter = 0;
                        predecessor = 'N';
                        break;
                case 2: x_counter = 0; //check west
                        y_counter = -1;
                        predecessor = 'E';
                        break;
                case 3: x_counter = 0; //check east
                        y_counter = 1;
                        predecessor = 'W';
                        break;
            }

            if(!bounded(wavefront_structure[0].x + x_counter, wavefront_structure[0].y + y_counter) && grid[wavefront_structure[0].x + x_counter][wavefront_structure[0].y + y_counter].reached != true)
            {
                grid[wavefront_structure[0].x + x_counter][wavefront_structure[0].y + y_counter].cellCost = grid[wavefront_structure[0].x][wavefront_structure[0].y].cellCost + 1;
                grid[wavefront_structure[0].x + x_counter][wavefront_structure[0].y + y_counter].predecessor = predecessor;
                grid[wavefront_structure[0].x + x_counter][wavefront_structure[0].y + y_counter].reached = true;

                wavefront_structure.push_back(input_data());
                w = wavefront_structure.end() - 1;

                w->target = 'c';
                w->x = wavefront_structure[0].x + x_counter;
                w->y = wavefront_structure[0].y + y_counter;
            }
        }

        //delete current cell from wavefront
        wavefront_structure.erase(wavefront_structure.begin());
    }
}

//Function to check if out of bounds
bool bounded(int x, int y)
{
    if(x < 0 || x > max_length - 1)
        return true;
    else if(y < 0 || y > max_length - 1)
        return true;
    else
        return false;
}
