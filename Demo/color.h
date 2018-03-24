/*
*   Copyright (c) 2002
*
*   Made by Petter Strandmark ("Sang-drax")
*   http://www.strandmark.com
*   Needed for functionality : <windows.h>
*
*/

/*
*  USAGE:
*
*  cout << RED << "This is red." << BLUE << "\nThis is blue.";
*
*
*/

#ifndef COLOR_PETTER_H
#define COLOR_PETTER_H


//Retain ANSI/ISO Compability
#ifdef WIN32
    #include <windows.h>
#endif

#include <iostream>


namespace Petter
{
    
    namespace
    {
        class Color
        {
            friend std::ostream& operator<<(std::ostream& stream,const Color& c);

        public:
            Color(unsigned short c): color(c) {}
            static bool IsAvailabe()
            {
                #ifdef WIN32
                    return true;
                #else
                    return false;
                #endif   
            }
        //private:
            unsigned short color;
        };

        std::ostream& operator<<(std::ostream& stream,const Color& c)
        {
            stream.flush();
            #ifdef WIN32
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),c.color);
            #endif
            stream.flush();

            return stream;
        }

        #ifdef WIN32
            const Color NORMAL  = FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE;
            const Color WHITE   = FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY;
            const Color RED     = FOREGROUND_RED|FOREGROUND_INTENSITY;
            const Color DKRED     = FOREGROUND_RED;
            const Color BLUE    = FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
            const Color DKBLUE    = FOREGROUND_BLUE|FOREGROUND_GREEN;
            const Color GREEN   = FOREGROUND_GREEN|FOREGROUND_INTENSITY;
            const Color DKGREEN   = FOREGROUND_GREEN;
            const Color YELLOW  = FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY;
            const Color BROWN   = FOREGROUND_RED|FOREGROUND_GREEN;
            
        #else
            const Color NORMAL  = 0;
            const Color WHITE   = 0;
            const Color RED     = 0;
            const Color DKRED     = 0;
            const Color BLUE    = 0;
            const Color DKBLUE    = 0;
            const Color GREEN   = 0;
            const Color DKGREEN   = 0;
            const Color YELLOW  = 0;
            const Color BROWN   = 0;
        #endif
    }

}
#endif //ifndef 
