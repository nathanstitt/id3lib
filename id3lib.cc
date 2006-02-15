#include "id3lib.h"

using namespace std;

struct strhash { gnu::hash<const char*> h;	inline size_t operator()(const std::string& key) const { return   h( key.c_str() ); } };

typedef gnu::hash_map<std::string,ID3_FrameID,strhash> frame_names_hash_t;

static frame_names_hash_t frames_hash;


static VALUE rb_id3lib;
static VALUE rb_id3pic;

static void delete_tag(ID3_Tag *ptr){ delete ptr; }

static const ID rb_method_to_s = rb_intern("to_s");
static const ID rb_method_new = rb_intern("new");
static const ID rb_method_to_i = rb_intern("to_i");




void
dump_frame( ID3_Frame *frame ){
  cout << frame->GetTextID() << "   " << frame->GetDescription() << endl;
  ID3_Frame::Iterator* fiter = frame->CreateIterator();
  ID3_Field* field = NULL;
  int i = 0;
  while ( NULL != ( field = fiter->GetNext()) ) {
    cout << "       " << ++i;
    uint32_t type=field->GetType();
    cout << " Type :" << type 
	 << " ID: "   << setw(4) << field->GetID()
	 << " Size: " << setw(4) << field->Size()
	 << "  -  ";
    switch ( type ){
    case 2:
      cout << field->GetRawText();
      break;
    case 1:
      cout << field->Get();
      break;
    case 3:
      cout << "BINARY";
      break;
    default:
      cout << "UNKNOWN TYPE";
      break;
    };
    cout << endl;
  }
}



static VALUE
id3lib_save( VALUE self ){
  ID3_Tag *tag;
  Data_Get_Struct( self, ID3_Tag, tag );
  tag->Update();
  return Qtrue;
}



static VALUE
id3lib_init( VALUE self, VALUE file ) {
  ID3_Tag *tag;
  Data_Get_Struct( self, ID3_Tag, tag );
  tag->Link( StringValuePtr(file) );
  if ( ! tag->GetFileSize() ) {
    rb_raise ( rb_eArgError, "File not opened correctly");
  }
  return self;
}



static VALUE
id3lib_new( VALUE cls, VALUE file ){
  VALUE argv[1];
  argv[0]=file;
  ID3_Tag *tag = new ID3_Tag();
  VALUE self = Data_Wrap_Struct(rb_id3lib, 0, delete_tag, tag );
  rb_obj_call_init( self, 1, argv );
  return self;
}



VALUE
id3lib_add_picture( VALUE self, VALUE data, VALUE type ){
  ID3_Tag *tag;
  Data_Get_Struct( self, ID3_Tag, tag );
  long int len;
  char *dat=rb_str2cstr( data, &len );
  ID3_Frame *frame = new ID3_Frame( ID3FID_PICTURE );
  frame->GetField( ID3FN_DATA )->Set( (const uchar*)dat, len );
  frame->GetField( ID3FN_PICTURETYPE )->Set( INT2FIX( type ) );
  tag->AttachFrame( frame );
  return Picture::Ruby( self, frame );
}


static VALUE
id3lib_each_picture( VALUE self ){
  ID3_Tag *tag;
  Data_Get_Struct( self, ID3_Tag, tag );
  std::auto_ptr<ID3_Tag::Iterator> iter( tag->CreateIterator() );
  ID3_Frame* frame = NULL;
  while (NULL != ( frame = iter->GetNext())){
    if ( frame->GetID() == ID3FID_PICTURE ) {
      rb_yield( Picture::Ruby( self, frame ) );
    }
  }
  return Qtrue;
}

static VALUE
id3lib_pictures( VALUE self ){
  ID3_Tag *tag;
  Data_Get_Struct( self, ID3_Tag, tag );
  VALUE ret=rb_ary_new();
  std::auto_ptr<ID3_Tag::Iterator> iter( tag->CreateIterator() );
  ID3_Frame* frame = NULL;
  while (NULL != ( frame = iter->GetNext())){
    if ( frame->GetID() == ID3FID_PICTURE ) {
      rb_ary_push( ret, Picture::Ruby( self, frame ) );
    }
  }
  return ret;
}





ID3_Frame*
get_comments_frame( ID3_Tag *tag ){
  ID3_Frame* frame = tag->Find(ID3FID_COMMENT, ID3FN_DESCRIPTION, "Comments");
  if ( ! frame ){
    frame = tag->Find(ID3FID_COMMENT, ID3FN_DESCRIPTION, "ID3v1 Comment");
  }
  return frame;
}

static VALUE
id3lib_set_comment( VALUE self,VALUE value ){
  ID3_Tag *tag;
  Data_Get_Struct( self, ID3_Tag, tag );
  char *val;
  long int len;
  if ( ! NIL_P( value ) ){
    val=rb_str2cstr(value,&len);
  }
  ID3_Frame* frame = get_comments_frame( tag );
  if ( frame) {
    if ( NIL_P( value ) ){
      frame->GetField( ID3FN_TEXT )->Set( "" );
    } else {
      frame->GetField( ID3FN_TEXT )->Set( val );
    }
  }
  return value;
}


static VALUE
id3lib_get_comment( VALUE self ){
  ID3_Tag *tag;
  Data_Get_Struct( self, ID3_Tag, tag );
  VALUE ret=Qnil;
  ID3_Frame* frame = get_comments_frame( tag );
  if ( frame) {
    ret=rb_str_new2( frame->GetField( ID3FN_TEXT )->GetRawText() );
  }
  return ret;
}


VALUE
id3lib_each_possible_tag( VALUE self ){
  ID3_Tag *t;
  Data_Get_Struct( self, ID3_Tag, t );
  for ( frame_names_hash_t::const_iterator tag=frames_hash.begin();
	tag != frames_hash.end(); ++tag ) {

    VALUE desc=Qnil;
    ID3_Frame *frame=t->Find( tag->second );
    if ( frame ){
      desc=rb_str_new2( frame->GetDescription() );
    }
    rb_yield( rb_ary_new3( 2, rb_str_new2( tag->first.c_str() ), desc ) );
  }

  return Qtrue;
}


VALUE
id3lib_debug( VALUE self ){
  ID3_Tag *tag;
  Data_Get_Struct( self, ID3_Tag, tag );
  //  tag->SetSpec( ID3V2_2_0 );
  cout << "SPEC: " << tag->GetSpec() << endl;
  cout << tag->NumFrames() << " Frames" << endl;

  std::auto_ptr<ID3_Tag::Iterator> iter( tag->CreateIterator() );
  ID3_Frame* frame = NULL;
  while (NULL != ( frame = iter->GetNext())){
    dump_frame( frame );
  }
  return Qtrue;
}

static VALUE
id3lib_get_str_tag( VALUE self, ID3_FrameID frame_id,ID3_FieldID field_id ){
  ID3_Tag *tag;
  Data_Get_Struct( self, ID3_Tag, tag );
  VALUE ret=Qnil;
  ID3_Frame* frame = tag->Find( frame_id );
  if ( frame) {
    ret=rb_str_new2( frame->GetField( field_id )->GetRawText() );
  }
  return ret;
}

static VALUE
id3lib_set_str_tag( VALUE self, VALUE value, ID3_FrameID frame_id, ID3_FieldID field_id ){
  ID3_Tag *tag;
  Data_Get_Struct( self, ID3_Tag, tag );
  long int len;
  char *val;
  val=rb_str2cstr(value,&len);
  ID3_Frame* frame = tag->Find( frame_id );
  if ( frame) {
    frame->GetField( field_id  )->Set( val );
  } else {
    ID3_Frame *frame = new ID3_Frame( frame_id );
    frame->GetField( field_id )->Set( val );
    tag->AttachFrame( frame );
  }
  return value;
}


static VALUE
id3lib_set_int_tag( VALUE self, VALUE value, ID3_FrameID frame_id, ID3_FieldID field_id ){
  ID3_Tag *tag;
  Data_Get_Struct( self, ID3_Tag, tag );
  int32_t val = NUM2INT( value );
  ID3_Frame* frame = tag->Find( frame_id );
  if ( frame) {
    frame->GetField( field_id )->Set( val );
  } else {
    ID3_Frame *frame = new ID3_Frame( frame_id );
    frame->GetField( field_id )->Set( val );
    tag->AttachFrame( frame );
  }
  return value;
}


static VALUE
id3lib_get_int_tag( VALUE self, ID3_FrameID frame_id, ID3_FieldID field_id ){
  ID3_Tag *tag;
  Data_Get_Struct( self, ID3_Tag, tag );
  VALUE ret=INT2FIX( 0 );
  ID3_Frame* frame = tag->Find( frame_id );
  if ( frame) {
    ret=INT2NUM( frame->GetField( field_id )->Get() );
  }
  return ret;
}


static VALUE
id3lib_get_url( VALUE self ){
  return id3lib_get_str_tag( self, ID3FID_WWWUSER, ID3FN_URL );
}

static VALUE
id3lib_set_url( VALUE self, VALUE val ){
  return id3lib_set_str_tag( self, val, ID3FID_WWWUSER, ID3FN_URL );
}


static VALUE
id3lib_set_popularity_meter( VALUE self, VALUE value ){
  return id3lib_set_int_tag( self, value, ID3FID_POPULARIMETER, ID3FN_COUNTER );
}

static VALUE
id3lib_get_popularity_meter( VALUE self ){
  return id3lib_get_int_tag( self, ID3FID_POPULARIMETER, ID3FN_COUNTER );
}

static VALUE
id3lib_set_play_counter( VALUE self, VALUE value ){
  return id3lib_set_int_tag( self, value, ID3FID_PLAYCOUNTER, ID3FN_COUNTER );
}

static VALUE
id3lib_get_play_counter( VALUE self ){
  return id3lib_get_int_tag( self, ID3FID_PLAYCOUNTER, ID3FN_COUNTER );
}


VALUE
str_set( VALUE self, VALUE val ){
  std::string method_name( rb_id2name(rb_frame_last_func()) );
  method_name.resize( method_name.size()-1 );
  ID3_FrameID frame_id=frames_hash[ method_name ];
  return id3lib_set_str_tag( self, val, frame_id, ID3FN_TEXT );
}

VALUE
str_get( VALUE self ){
  std::string method_name( rb_id2name(rb_frame_last_func()) );
  ID3_FrameID frame_id=frames_hash[ method_name ];
  return id3lib_get_str_tag( self, frame_id, ID3FN_TEXT );
}

void
define_string_method( const char  *name, ID3_FrameID id ){
  std::string eq_name( name );
  eq_name+='=';
  frames_hash[ name ] = id;

  rb_define_method( rb_id3lib, name, RB_METHOD( str_get ), 0 );
  rb_define_method( rb_id3lib, eq_name.c_str(), RB_METHOD( str_set ), 1 );
}


extern "C"
void
Init_id3lib(){

  rb_id3lib = rb_define_class("ID3lib", rb_cObject );

  rb_id3pic=init_pic( rb_id3lib );

  
  rb_define_singleton_method(rb_id3lib, "new", RB_METHOD(id3lib_new), 1);

  // these we have to handle differently
  frames_hash[ "comment"  ] = ID3FID_COMMENT;
  frames_hash[ "play_counter" ] = ID3FID_PLAYCOUNTER;
  frames_hash[ "popularity_meter" ] = ID3FID_POPULARIMETER;
  frames_hash[ "url" ] = ID3FID_WWWUSER;


  rb_define_method(rb_id3lib, "initialize", RB_METHOD(id3lib_init), 1);
  rb_define_method(rb_id3lib, "save", RB_METHOD( id3lib_save ), 0);

  rb_define_method( rb_id3lib, "each_possible_tag", RB_METHOD( id3lib_each_possible_tag ), 0);
  // add a picture
  rb_define_method( rb_id3lib, "add_picture", RB_METHOD( id3lib_add_picture ), 2 );
  rb_define_method( rb_id3lib, "each_picture", RB_METHOD( id3lib_each_picture ), 0 );
  rb_define_method( rb_id3lib, "pictures", RB_METHOD( id3lib_pictures ), 0 );
  rb_define_method( rb_id3lib, "debug", RB_METHOD( id3lib_debug ), 0 );
  rb_define_method( rb_id3lib, "comment", RB_METHOD( id3lib_get_comment ), 0 );
  rb_define_method( rb_id3lib, "comment=", RB_METHOD( id3lib_set_comment ), 1 );

  rb_define_method( rb_id3lib, "play_counter",  RB_METHOD( id3lib_get_play_counter), 0 );
  rb_define_method( rb_id3lib, "play_counter=", RB_METHOD( id3lib_set_play_counter), 1 );
  
  rb_define_method( rb_id3lib, "popularity_meter",  RB_METHOD( id3lib_get_popularity_meter), 0 );
  rb_define_method( rb_id3lib, "popularity_meter=", RB_METHOD( id3lib_set_popularity_meter), 1 );

  rb_define_method( rb_id3lib, "url", RB_METHOD( id3lib_get_url), 0 );
  rb_define_method( rb_id3lib, "url=", RB_METHOD( id3lib_set_url), 1 );

  define_string_method( "involved_people", ID3FID_INVOLVEDPEOPLE );
  define_string_method( "linked_info", ID3FID_LINKEDINFO );
  define_string_method( "album", ID3FID_ALBUM );
  define_string_method( "bpm", ID3FID_BPM );
  define_string_method( "composer", ID3FID_COMPOSER );
  define_string_method( "content_type", ID3FID_CONTENTTYPE );
  define_string_method( "copyright", ID3FID_COPYRIGHT );
  define_string_method( "date" , ID3FID_DATE );
  define_string_method( "playlist_delay", ID3FID_PLAYLISTDELAY );
  define_string_method( "encoded_by", ID3FID_ENCODEDBY );
  define_string_method( "lyricist", ID3FID_LYRICIST );
  define_string_method( "file_type", ID3FID_FILETYPE );
  define_string_method( "time", ID3FID_TIME );
  define_string_method( "content_group", ID3FID_CONTENTGROUP );
  define_string_method( "title", ID3FID_TITLE );
  define_string_method( "subtitle", ID3FID_SUBTITLE );
  define_string_method( "initial_key", ID3FID_INITIALKEY );
  define_string_method( "language", ID3FID_LANGUAGE );
  define_string_method( "songlen", ID3FID_SONGLEN );
  define_string_method( "mediatype", ID3FID_MEDIATYPE );
  define_string_method( "orig_album", ID3FID_ORIGALBUM );
  define_string_method( "filename", ID3FID_ORIGFILENAME );
  define_string_method( "orig_lyricist", ID3FID_ORIGLYRICIST );
  define_string_method( "orig_artist", ID3FID_ORIGARTIST );
  define_string_method( "orig_year", ID3FID_ORIGYEAR );
  define_string_method( "file_owner", ID3FID_FILEOWNER );
  define_string_method( "artist", ID3FID_LEADARTIST );
  define_string_method( "band",ID3FID_BAND );
  define_string_method( "conductor",ID3FID_CONDUCTOR );
  define_string_method( "mix_artist",ID3FID_MIXARTIST );
  define_string_method( "partinset",ID3FID_PARTINSET );
  define_string_method( "publisher", ID3FID_PUBLISHER );
  define_string_method( "track", ID3FID_TRACKNUM );
  define_string_method( "recording_dates", ID3FID_RECORDINGDATES );
  define_string_method( "net_radio_station", ID3FID_NETRADIOSTATION );
  define_string_method( "net_radio_owner", ID3FID_NETRADIOOWNER );
  define_string_method( "size", ID3FID_SIZE );
  define_string_method( "isrc", ID3FID_ISRC );
  define_string_method( "encoder_settings",ID3FID_ENCODERSETTINGS );
  define_string_method( "user_text",ID3FID_USERTEXT );
  define_string_method( "year",ID3FID_YEAR );
  define_string_method( "terms_of_use",ID3FID_TERMSOFUSE );
  define_string_method( "unsynced_lyrics", ID3FID_UNSYNCEDLYRICS );

}
