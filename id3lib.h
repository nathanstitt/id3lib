#include <ruby.h>
#include <id3/tag.h>
#include <string>
#include <ext/hash_map>
#include <iostream>
#include <iomanip>

#ifndef _ID3LIB_H_
#define _ID3LIB_H_

#define RB_METHOD(func) ((VALUE (*)(...))func)

VALUE init_pic( VALUE  parent );

namespace gnu = __gnu_cxx;

class Picture {
 private:
  Picture( VALUE parent, ID3_Frame * );
 public:
  static VALUE Ruby( VALUE parent, ID3_Frame* );
  VALUE parent;
  ID3_Frame *frame;
  ~Picture();
};


#endif // _ID3LIB_H_
