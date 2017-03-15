
#include "ClientNetwork.h"

#include "NTL/ZZ.h"

#include <iostream>

using namespace NTL;
using namespace std;

int main(int argc, char** argv)
{
    if (argc != 2)
    {
      std::cerr << "Usage: UserInput <host>" << std::endl;
      return 1;
    }

    ClientNetwork cnet;
    cnet.Connect(argv[1]);

    uint32_t requestNumber= 0;

    while(1)
    {
        ZZ input;
        cout << "Enter a number to check for primality: ";
        cin >> input;

        PrimeCheckRequest req;
        req.key = requestNumber++;
        req.number = input;

        cnet.SendRequest(req);

        cnet.ReadResults();

        std::list<std::pair<uint32_t,bool>> results = cnet.GetResults();

        for(auto res : results)
        {
            if(res.second == true)
                cout << input << " is prime" << endl;
            else
                cout << input << " is not prime" << endl;
        }

    }

}