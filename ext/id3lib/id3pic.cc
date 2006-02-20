
#include "id3lib.h"

static VALUE rb_id3pic;


static const ID rb_method_to_s = rb_intern("to_s");

Picture::Picture(  VALUE p, ID3_Frame *f  ):
  parent( p ),
  frame( f )
{ }

Picture::~Picture(){
  
}

static void delete_pic(Picture *ptr){ delete ptr; }
static void mark_pic(Picture *ptr){ rb_gc_mark( ptr->parent ); }


VALUE
Picture::Ruby(  VALUE parent, ID3_Frame *f ){
  Picture  *p=new Picture( parent, f );
  VALUE pic = Data_Wrap_Struct( rb_id3pic, mark_pic, delete_pic, p );
  return pic;
}

VALUE
set_description( VALUE self, VALUE str ){
  Picture *pic;
  Data_Get_Struct( self, Picture, pic );
  pic->frame->GetField( ID3FN_DESCRIPTION )->Set( STR2CSTR( rb_funcall( str, rb_method_to_s,0 ) ) );
  return str;
}


VALUE
get_description( VALUE self ){
  Picture *pic;
  Data_Get_Struct( self, Picture, pic );
  return rb_str_new2( pic->frame->GetField( ID3FN_DESCRIPTION )->GetRawText() );
}

VALUE
rm_pic( VALUE self ){
  Picture *pic;
  Data_Get_Struct( self, Picture, pic );
  pic->frame->Clear();
  return Qtrue;
}

VALUE
get_depicts( VALUE self ){
  Picture *pic;
  Data_Get_Struct( self, Picture, pic );
  return INT2FIX( pic->frame->GetField( ID3FN_PICTURETYPE )->Get()  );
}

VALUE
set_depicts( VALUE self, VALUE id ){
  Picture *pic;
  Data_Get_Struct( self, Picture, pic );
  pic->frame->GetField( ID3FN_PICTURETYPE )->Set( NUM2INT( id ) );
  return id;
}

VALUE
get_data( VALUE self ){
  Picture *pic;
  Data_Get_Struct( self, Picture, pic );
  ID3_Field *f=pic->frame->GetField( ID3FN_DATA );
  return rb_str_new( (const char*)f->GetRawBinary(), f->BinSize() );
}

VALUE
set_data( VALUE self, VALUE data ){
  Picture *pic;
  Data_Get_Struct( self, Picture, pic );
  ID3_Field *f=pic->frame->GetField( ID3FN_DATA );
  long int len;
  char *dat=rb_str2cstr( data, &len );
  f->Set( (const uchar*)dat, len );
  return data;
}

void
make_const( const char *name, uint32_t id ){
  rb_define_const( rb_id3pic, name, INT2NUM( id ) );
}

VALUE
init_pic( VALUE rb_id3lib ){
  rb_id3pic = rb_define_class_under( rb_id3lib, "Picture", rb_cObject );

  make_const("PNG_ICON", 1);
  make_const("FILE_ICON", 2);
  make_const("FRONT_COVER", 3);
  make_const("BACK_COVER", 4);
  make_const("LEAFLET_PAGE", 5);
  make_const("MEDIA", 6);
  make_const("LEAD_ARTIST", 7);
  make_const("ARTIST", 8);
  make_const("CONDUCTOR", 9);
  make_const("BAND", 10);
  make_const("COMPOSER", 11);
  make_const("LYRICIST", 12);
  make_const("RECORDING_LOCATION", 13);
  make_const("DURING_RECORDING", 14);
  make_const("DURING_PERFORMANCE", 15);
  make_const("SCREEN_CAPTURE", 16);
  // NO IDEA.  I've taken all these tag names from easytag, and that's what
  // they have for # 17
  make_const("BRIGHTLY_COLORED_FISH", 17);
  make_const("ILLUSTRATION", 18);
  make_const("BAND_LOGO", 19);
  make_const("PUBLISHER_LOGO", 20);

  rb_define_method( rb_id3pic, "delete", RB_METHOD( rm_pic ), 0 );
  rb_define_method( rb_id3pic, "description", RB_METHOD( get_description ), 0 );
  rb_define_method( rb_id3pic, "description=", RB_METHOD( set_description ), 1 );
  rb_define_method( rb_id3pic, "data", RB_METHOD( get_data ), 0 );
  rb_define_method( rb_id3pic, "data=", RB_METHOD( set_data ), 1 );
  rb_define_method( rb_id3pic, "depicts", RB_METHOD( get_depicts ), 0 );
  rb_define_method( rb_id3pic, "depicts=", RB_METHOD( set_depicts ), 1 );
  
  return rb_id3pic;
}
