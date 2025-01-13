#include <cstddef>
#include <ios>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <algorithm>
#include <fstream> 
#include <cctype>

namespace fs = std::filesystem;

std::vector<std::string> parseInput(const std::string & input);
std::string findCommandInPath(const std::string & command);
bool isBuiltIn(const std::string &command, const std::vector <std::string> &builtins);
void handleTypeCommand(const std::string &command, const std::vector <std::string> &builtins);
void executeExternalCommands(std::vector <std::string> &args);
void handleCd(std::vector <std::string>& myvec);
void handleCat(std::vector <std::string> &args);
int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::vector <std::string> myvec = {"type", "echo", "exit", "pwd", "cd", "cat"};
  bool isValid = false;
  while(!isValid){
        
      std::cout << "$ ";
      std::string input;
      
      //std::string targetEcho = "echo";
      //std::string targetType = "type";
      //bool echoTrue = true;
      //bool typeTrue = true;
      std::getline(std::cin, input);
       /*   
      for(int i = 0; i < 4; i++){
        if(input[i] != targetEcho[i]){
            echoTrue = false;
        }
        if(input[i] != targetType[i]){
            typeTrue = false;
        }
      }*/
      if(input.empty()){
        continue;
      }

      std::vector <std::string> args = parseInput(input);
      if(args.empty()){
        continue;
      }
        

      if(input == "exit 0"){
        isValid = true;
        break;
      }


      if(args[0] == "echo"){
        for(size_t i = 1; i < args.size(); i++){
            std::cout << args[i] << " ";
        }
        std::cout << std::endl;
        continue;
      }

      if(args[0] == "type" && args.size() > 1){
        
        std::size_t start_index = 5; 
        handleTypeCommand(args[1], myvec);
        continue;

      }

      if(args[0] == "pwd"){
        fs::path currPath = fs::current_path();
        std::cout << currPath.string() << std::endl;
        continue; 

      }

      if(args[0] == "cd"){
        handleCd(args);
        continue;
      }
    
      if(args[0] == "cat"){
        handleCat(args);
        continue; 


      }

   executeExternalCommands(args);   

      
       

  }
}


bool isBuiltIn(const std::string &command, const std::vector <std::string> &builtins){
    return std::find(builtins.begin(), builtins.end(), command) != builtins.end(); 
}

void handleTypeCommand(const std::string &command, const std::vector <std::string> &builtins){
   if(isBuiltIn(command, builtins)){
       if(command == "cat"){
           std::string currPath = findCommandInPath(command);

           std::cout << command << " is " << currPath << std::endl;

       }else{

        std::cout << command << " is a shell builtin" << std::endl;
       }
   }else{
    std::string path = findCommandInPath(command);

    if(!path.empty()){
        std::cout << command << " is " << path << std::endl;
    }else{
        std::cout << command << " not found" << std::endl;
    }
   }
    

}

std::string findCommandInPath(const std::string & command){
    const char * pathEnv = std::getenv("PATH");
    if(!pathEnv){
      return "PATH environment variable not set \n";   
    }  
    
    std::string pathStr(pathEnv); 
    size_t start = 0, end = 0;
    
    while((end = pathStr.find(':', start)) != std::string::npos){
        std::string dir = pathStr.substr(start, end - start);
        fs::path filePath = fs::path(dir) / command;


        if(fs::exists(filePath) && fs::is_regular_file(filePath) 
        &&( fs::status(filePath).permissions() & fs::perms::owner_exec) != fs::perms::none ){
           return filePath.string(); 


        }

        start = end + 1;

    }

    fs::path filePath = fs::path(pathStr.substr(start)) /command;

    if(fs::exists(filePath) && fs::is_regular_file(filePath) 
    &&(fs::status(filePath).permissions() & fs::perms::owner_exec) != fs::perms::none){
        return filePath.string();
    }
    
    return "";
}

void executeExternalCommands(std::vector <std::string> &args){
        
      std::vector <char *> charArgs; 

      for(auto &elem: args){
        charArgs.push_back(&elem[0]); 
      }

      charArgs.push_back(nullptr);

      pid_t pid = fork();

      if(pid == 0){
        execvp(charArgs[0], charArgs.data()); 
        for(auto &elem: args){
            std::cout << elem << ": ";
        } 
            std::cout << "command not found" << std::endl;
             
        exit(1); 

      }else if(pid > 0){
        int status;
        waitpid(pid, &status, 0); //wait for child process to finish
        
      }else{
        std::cerr << "fork failed" << std::endl;
      }

}


void handleCd(std::vector <std::string> &args){
    if(args.size() < 2){
        std::cerr << "cd: missing argument" << std::endl; 
        return;
    } 

    std::string targetPath = args[1];
    if(targetPath == "~" || targetPath.find("~/") == 0){
       const char * home = std::getenv("HOME");
        if(home){
            if(targetPath == "~"){
                targetPath = std::string(home); 
            }else{
                targetPath = std::string(home) + targetPath.substr(1);
            }

        }else{
            std::cerr << "cd: HOME environment variable not set" <<std::endl; 
            return;
        }

    }
    fs::path newPath(targetPath); 
    if(fs::exists(newPath) && fs::is_directory(newPath)){
        fs::current_path(newPath);
    }else{
        std::cerr << "cd: " << targetPath << ": No such file or directory" << std::endl;            
    }

}


void handleCat(std::vector <std::string> &args){
    std::ostringstream output;
    for(size_t i = 1; i < args.size(); i++){
        std::ifstream myfile(args[i]);
        if(myfile.is_open()){
            std::cout << myfile.rdbuf();
        }else{
            std::cerr << "cat: " << args[i] << ": No such File or directory" << std::endl;
            return;
        }

    }

}


std::vector<std::string> parseInput(const std::string & input){


      std::istringstream iss(input);
      std::vector <std::string> args; 
      std::string currentArg; 

      bool inSingleQuote = false; 
      bool inDoubleQuote = false;
      for(size_t i = 0; i < input.size(); ++i){
        char c = input[i];

        if(c == '\'' && !inDoubleQuote){
            //if quote is found we make it true
           inSingleQuote = !inSingleQuote; 
        }else if(c == '"' && !inSingleQuote){
            inDoubleQuote = !inDoubleQuote; 
            
        }else if(c == '\\'){
            if(i + 1 < input.size()){
                char nextChar = input[i+1]; 
                if(inDoubleQuote && (nextChar == '\\' || nextChar == '$' || nextChar == '"')){
                    currentArg += nextChar;
                    ++i;
                }else if(!inSingleQuote){
                    currentArg += nextChar; 
                    ++i;
                }else{
                    currentArg += c;
                }
            }else{
                currentArg += c;
            }
        }else if(std::isspace(c) && !inSingleQuote &&!inDoubleQuote){
            //if space is encountered and quote is not found we append the argument to the array
            if(!currentArg.empty()){
                args.push_back(currentArg);
                currentArg.clear();
            }

        }else{
            //argument takes in 
            currentArg += c;

        }

      }

      if(!currentArg.empty()){
        args.push_back(currentArg);
      }
        
      if(inSingleQuote || inDoubleQuote){
        std::cerr << "unmatched quote in input" << std::endl;
      }
      
    return args;
}
