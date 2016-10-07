#include <io.h>   // For access().
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "generate-html-file.h"


void
GenerateHtmlFile::Generate(int fileSize)
{
  std::ofstream myfile;
  std::string strPath = "files-2";
  if ( _access( strPath.c_str(), 0 ) == 0 )
   {
      struct stat status;
      stat( strPath.c_str(), &status );

      if ( status.st_mode & S_IFDIR )
      {
		  std::cout << "The directory exists." << std::endl;
		  myfile.open ("files-2\\example.html");

		  myfile << "<http>\n <body>\n";
		  std::vector<char> empty(1024, 0);

		   for(int i = 0; i < 1024*fileSize; i++)
			{
				if (!myfile.write(&empty[0], empty.size()))
				{
					std::cerr << "problem writing to file" << std::endl;
				}
			}

		  myfile << "Dummy html file\n";
		  myfile << "</body>\n</http>\n";
		  myfile.close();
      }
      else
      {
         std::cout << "The path you entered is a file." << std::endl;
      }
   }
   else
   {
      std::cout << "Path doesn't exist." << std::endl;
   }
  
}
