#include <cstddef>
#include <iostream>
#include <vector>
#include <string>
int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::vector <std::string> myvec = {"type", "echo", "exit"};
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
      

      if(input == "exit 0"){
        isValid = true;
        break;
      }else if(echoTrue){
        for(int i = 5; i < input.size(); i++){
            std::cout << input[i];
        }
        std::cout << std::endl;
      }else if(typeTrue){
        std::size_t start_index = 5; 
        std::string substring = input.substr(start_index);
        bool isBuiltIn = false; 

        for(auto &elem: myvec){
            if(substring.find(elem) != std::string::npos){
                isBuiltIn = true;
                break;
            }

        } 

        if(isBuiltIn){
            std::cout << substring << " is a shell builtin" << std::endl;
        }else{
            std::cout << substring << " not found" << std::endl;
        }


      }else{


        std::cout << input << ": command not found" << std::endl;
      }


      

      
       

  }
}
