#pragma once

#include <string_view>
#include <optional>
#include <iterator>
#include <atomic>

using namespace std::literals;

struct my_line_iterator {
   using iterator_category = std::input_iterator_tag;
   using value_type = std::string_view;
   using difference_type = std::ptrdiff_t;
   using reference_type = const value_type&;
   using pointer_type = const value_type*;

   my_line_iterator() { };
   my_line_iterator(std::string_view const& input) : theText(input) { ++* this; };

   my_line_iterator& operator = (std::string_view const& vw) {
      theText = vw;
      start_pos = 0u;
      end_pos = 0u;
      return *this;
   }

   my_line_iterator& operator = (my_line_iterator const& ref) {
      theText = ref.theText;
      theLine = ref.theLine;
      start_pos = ref.start_pos;
      end_pos = ref.end_pos;
      return *this;
   }

   reference_type operator*() const { return theLine; }
   pointer_type operator->() const { return &theLine; }

   my_line_iterator& operator++() {
      if (theText) {
         end_pos = theText->find('\n', start_pos);
         if (end_pos != std::string_view::npos) {
            theLine = theText->substr(start_pos, end_pos - start_pos);
            start_pos = end_pos + 1;
         }
         else {
            //theText = { };
            theText = std::nullopt;
         }
      }
      return *this;
   }

   my_line_iterator operator++(int) {
      auto elem(*this);
      ++* this;
      return elem;
   }

   friend  bool operator == (my_line_iterator const& lhs, my_line_iterator const& rhs) {
      return lhs.theText == rhs.theText;
   }

   friend  bool operator != (my_line_iterator const& lhs, my_line_iterator const& rhs) {
      return !(lhs == rhs);
   }

private:
   std::optional<std::string_view> theText = { };
   std::string_view theLine;
   size_t start_pos = 0u, end_pos = 0u;
};


struct my_lines {
   my_lines(std::string_view const& input) { theText = input;  }
   my_lines(my_lines const& ref) { theText = ref.theText;  }

   my_line_iterator begin() const { return my_line_iterator(theText); }
   my_line_iterator end() const { return my_line_iterator(); }


   std::string_view theText;

};

struct my_line {
   my_line(void) : view() { }
   my_line(std::string_view const& input) : view(input) { }
   std::string_view view;
};


struct my_line_count {

   my_line_count(void) : view(), index(counter++) { }
   my_line_count(std::string_view const& input) : view(input), index(counter++) { }

   std::string_view view;
   int index;

   static inline std::atomic<int> counter = 0;
   static int GetCounter(void) { return counter;  }
   static void reset(void) { counter = 0; }
};

