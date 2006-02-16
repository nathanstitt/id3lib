#!/usr/bin/ruby

require 'mkmf'

def crash(str)
  printf(" extconf failure: %s\n", str)
  exit 1
end

unless have_library('id3', 'ID3Tag_New')
  crash('need libid3tag')
end

$CFLAGS = '-Wall'
$LDFLAGS ='-lid3tag'

create_header()

create_makefile("id3lib")



arr=Array::new

File::open("Makefile","r"){|f| 
    f.each_line{ | l | 
        arr << l.gsub(/gcc/,'g++') 
    }
}

File::open("Makefile","w"){|f|
  arr.each{|l|
    f.puts(l)
  }
}

