#include <cstddef>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <algorithm>

namespace fs = std::filesystem;


std::string findCommandInPath(const std::string & command);
bool isBuiltIn(const std::string &command, const std::vector <std::string> &builtins);
void handleTypeCommand(const std::string &command, const std::vector <std::string> &builtins);
void executeExternalCommands(std::vector <std::string> &args);

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::vector <std::string> myvec = {"type", "echo", "exit", "pwd", "cd"};
  bool isValid = false;
  while(!isValid){
        
      std::cout << "$ ";
      std::string input;
      std::string targetEcho = "echo";
      std::string targetType = "type";
      bool echoTrue = true;
      bool typeTrue = true;
      std::getline(std::cin, input);
          
      for(int i = 0; i < 4; i++){
        if(input[i] != targetEcho[i]){
            echoTrue = false;
        }
        if(input[i] != targetType[i]){
            typeTrue = false;
        }
      }
      
      std::istringstream iss(input);
      std::vector <std::string> args; 
      std::string token; 

      while(iss >> token){
        args.push_back(token);
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
        std::string substring = input.substr(start_index);
        
        handleTypeCommand(substring, myvec);
        continue;

      }

      if(args[0] == "pwd"){
        fs::path currPath = fs::current_path();
        std::cout << currPath.string() << std::endl;
        continue; 

      }

      if(args[0] == "cd"){
        fs::path newPath(args[1]); 
        if(fs::exists(newPath) && fs::is_directory(newPath)){
            fs::current_path(newPath);
        }else{
            std::cerr << "cd: " << args[1] << ": No such file or directory" << std::endl;            
        }
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
    std::cout << command << " is a shell builtin" << std::endl;
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
