/*
 * utils.cpp
 *
 *  Created on: Mar 23, 2016
 *      Author: Nick Gaulke
 */

#include <sys/ioctl.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <vector>
#include "config.h"
#include "utils.h"

using namespace std;

string asset_loc = ASSET_LOC;

/*
 * Returns the width of the console
 */
size_t get_console_width() {
	struct winsize size;
	ioctl(STDOUT_FILENO,TIOCGWINSZ,&size);
	return size.ws_col;
}

/*
 * Returns the height of terminal
 */
size_t get_console_height() {
	struct winsize size;
	ioctl(STDOUT_FILENO,TIOCGWINSZ,&size);
	return size.ws_row;
}

/*
 * Loads the menu file of the given type to be used
 * for printing the UI
 */
string get_display_screen(string type) {
    // Open the assets file for the current screen
    ifstream screen_file(asset_loc+"/"+type);
    string ret = "";
    
    if (screen_file.is_open()) {
        string buffer;
        
        while (getline(screen_file, buffer)) {
            // Append the formatted line to the cumulative string
            if(screen_file.peek() != EOF)
            	ret = ret + buffer + '\n';
            else ret = ret + buffer;
        }
        screen_file.close();
    }
    else {
        // TODO: Raise an exception here, if an asset file
        // cannot be opened then something serious has gone wrong.
        cout << "[1] FILE " << asset_loc << "/" << type << " COULD NOT BE OPENED" << endl;
    }
    
    return ret;
}

/*
 * Converts the given file to an exact string copy
 * used to create images and other printed files.
 */
string load_file(string file) { 
	// Open the assets file for the current screen
    ifstream screen_file(asset_loc+"/"+file);
    string ret = "";
    
    if (screen_file.is_open()) {
        string buffer;
        
        while (getline(screen_file, buffer)) {
            ret = ret + buffer + '\n';
        }
        screen_file.close();
    }
    else {
        // TODO: Raise an exception here, if an asset file
        // cannot be opened then something serious has gone wrong.
         cout << "[2] FILE " << asset_loc << "/" << file << " COULD NOT BE OPENED" << endl;
    }
    
    return ret;
}

/* 
 * Example: str = "hi", len = "5", ch = '-'
 * Return: "---hi"
 */  
string leftpad(string str, int len, char ch) {
   int i = -1;
   
   len = len - str.size();
   if (len <= 0) return str; // doesn't need to pad

   while (++i < len) {
       str = ch + str;
   }
   
   return str;
}

/* 
 * Example: str = "hi", len = "5", ch = '-'
 * Return: "hi---"
 */ 
string rightpad(string str, int len, char ch) {
    int i = -1;
    
    len = len - str.size();
    if (len <= 0) return str; // doesn't need to pad
    
    while (++i < len) {
       str = str + ch;
   }
    
    return str;
}

// Taken from http://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
std::istream& safeGetline(std::istream& is, std::string& t)
{
    t.clear();

    // The characters in the stream are read one-by-one using a std::streambuf.
    // That is faster than reading them one-by-one using the std::istream.
    // Code that uses streambuf this way must be guarded by a sentry object.
    // The sentry object performs various tasks,
    // such as thread synchronization and updating the stream state.

    std::istream::sentry se(is, true);
    std::streambuf* sb = is.rdbuf();

    for(;;) {
        int c = sb->sbumpc();
        switch (c) {
        case '\n':
            return is;
        case '\r':
            if(sb->sgetc() == '\n')
                sb->sbumpc();
            return is;
        case EOF:
            // Also handle the case when the last line has no line ending
            if(t.empty())
                is.setstate(std::ios::eofbit);
            return is;
        default:
            t += (char)c;
        }
    }
}

/*
 * Prints the text contents of the given file
 */
bool print_file(string file){
    ifstream fileToPrint;
    fileToPrint.open(file);
    if(fileToPrint.is_open()){
        string line;
        while(getline(fileToPrint,line)){
            cout << line << "\n";
        }
        fileToPrint.close();
    }else{
        cout << "Error! Failed to load file.\n";
    }
    return 0;
}

//Parses text input into the console and determines the appropriate response/action
int parse(string in) {
    if (in.size() > 0){
        cout << "parsing..." << endl;//message to user that program is working to fulfill request
        vector<string> words; //parsed individual words
        string word; //temporary container for word being built
    
        for(int i = 0; (unsigned) i < in.size();i++){//standardizes inputs to ignore case
            in[i] = tolower(in[i]);
            
            if((in[i] < 123 && in[i] > 96) || (in[i] < 58 && in[i] > 47) || in[i] == '+' || in[i] == '-'){
                word += in[i];//pushes character to word
            }else if(word.size() > 0){
                words.push_back(word);//end of word
                word = {};//resets word
            }
        }
        
        if(word.size() > 0){//end of command
            words.push_back(word);//end of word
        }
        
        if (words.size() > 0){
            cout << "Words (" << words.size() << "):\n";
            for(int i = 0; (unsigned) i < words.size();i++){
                cout << words[i];
                if((unsigned) i != words.size() - 1){
                    cout << ", ";
                }
            }
            
            cout << endl;
            
            //simple commands, must be expanded on based on command content
            if(words[0] == "exit" || words[0] == "quit" || words[0] == "q"){//quit program
                cout << "Quitting program...\n";
                return 404;
            }else if(words[0] == "gen" || words[0] == "generate"){
                if(words.size() > 2){
                    //nameGenerator(words[1],"dwarf");
                    string cmd = "./generator " + words[1] + " " + words[2];
                    return system(cmd.c_str());
                }else{
                    cout << "Missing arguments!\n";
                }
            }else if(words[0] == "roll"){
                cout << "Preparing to roll some dice...\n";
                //roll(0);
                if(words.size() > 1){
                    string cmd = "./roll ";
                    for (int i = 1; (unsigned) i < words.size(); i++) {
                        cmd += words[i] + " ";
                    }
                    //string cmd = "./roll " + (string)words[1];
                    return system(cmd.c_str());
                }else{
                    cout << "missing arguments\n";
                }
                //return 20;
            }else{//default case
                cout << "Command not recognized!\n";
            }
            
            words = {};
            
        }else{
            cout << "No command!\n";
        }
    }else{
        cout << "No command!\n";
    }
    return 0;
}