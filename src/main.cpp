#include <iostream>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  bool isValid = false;
  while(!isValid){
        
      std::cout << "$ ";
      std::string input;
      std::string targetEcho = "echo";
      bool echoTrue = true;
      std::getline(std::cin, input);
          
      for(int i = 0; i < 4; i++){
        if(input[i] != targetEcho[i]){
            echoTrue = false;
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
      }else{


        std::cout << input << ": command not found" << std::endl;

      }


      

      
       

  }
}
