#pragma once

#include <iostream>
#include <vector>

class MyBufferStream : public std::streambuf {
private:
   char* szBuffer;
   char* itPos;
public:
   MyBufferStream(size_t iSize) {
      //Buffer.resize(iSize, '\0'); // eventuell reserve
      szBuffer = new char[iSize];
      itPos = szBuffer;
   };
   MyBufferStream(MyBufferStream const&) = delete;
   ~MyBufferStream(void) { delete [] szBuffer; }

   char* get() { return szBuffer; }
   size_t size() { return itPos - szBuffer; }

   virtual int_type overflow(int_type c) {
      if (c != EOF) {
         *itPos++ = static_cast<char>(c);
      }
      return c;
   }

};


/*
class MyBufferStream : public std::streambuf {
   private:
      std::vector<char> Buffer;
      std::back_insert_iterator <std::vector<char>> itPos;
   public:
      MyBufferStream(size_t iSize) {
         //Buffer.resize(iSize, '\0'); // eventuell reserve
         Buffer.reserve(iSize);
         itPos = std::back_insert_iterator <std::vector<char>>(Buffer);
         };
      MyBufferStream(MyBufferStream const&) = delete;
      ~MyBufferStream(void) { };

      std::vector<char>& get() { return Buffer; }

      virtual int_type overflow(int_type c) {
         if (c != EOF) {
            *itPos++ = c;   
            }
         return c;
         }

   };
*/
