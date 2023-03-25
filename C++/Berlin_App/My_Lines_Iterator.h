#pragma once

#include <string_view>
#include <iterator>
#include <optional>
#include <atomic>

struct my_line_iterator {
   using iterator_category = std::input_iterator_tag;
   using value_type = std::string_view;
   using difference_type = std::ptrdiff_t;
   using reference = const value_type&;
   using pointer = const value_type*;

   my_line_iterator() {}
   my_line_iterator(std::string_view input) : theText(input) { ++* this; }

   my_line_iterator& operator = (std::string_view const& vw) {
      theText = vw;
      start_pos = 0;
      end_pos = 0;
   }
   my_line_iterator& operator = (my_line_iterator const& ref) {
      theText = ref.theText;
      theLine = ref.theLine;
      start_pos = ref.start_pos;
      end_pos = ref.end_pos;
   }

   reference operator*() const { return theLine; }
   pointer operator->() const { return &theLine; }

   my_line_iterator& operator++() {
      if (theText) {
         end_pos = theText->find('\n', start_pos);
         if (end_pos != std::string_view::npos) {
            theLine = theText->substr(start_pos, end_pos - start_pos);
            start_pos = end_pos + 1;
         }
         else {
            // falls letzte Zeile nicht mit Enter abgeschlossen ist, wird es komplizierter
            //theLine = theText->substr(start_pos, end_pos - start_pos);
            theText = { };
         }
      }
      return *this;
   }

   my_line_iterator operator++(int) {
      auto copy(*this);
      ++* this;
      return copy;
   }

   friend bool operator==(const my_line_iterator& x, const my_line_iterator& y) {
      return x.theText == y.theText;
   }

   friend bool operator!=(const my_line_iterator& x, const my_line_iterator& y) {
      return !(x == y);
   }

private:
   std::optional<std::string_view> theText = { };
   std::string_view theLine;
   size_t start_pos = 0, end_pos = 0;
};

struct my_lines {
   my_lines(std::string_view input) { theText = input; }
   my_lines(my_lines const& ref) { theText = ref.theText; }
   my_line_iterator begin() const { return my_line_iterator(theText); }
   my_line_iterator end() const { return my_line_iterator(); }

private:
   std::string_view theText;
};

template <typename ty>
struct my_line {
   my_line(void) : view(), data() {}
   my_line(std::string_view const& i) : view(i), data() {}
   operator ty && () { return std::forward<ty>(data); }

   std::string_view view;
   ty          data;
};

struct my_line_count {
   my_line_count(void) : view(), index(counter++) {}
   my_line_count(std::string_view const& i) : view(i), index(counter++) {}

   std::string_view view;
   int         index;
   static inline std::atomic<int>  counter = 0;
   static int GetCounter(void) { return counter; }
   static void reset() { counter = 0; }
};

