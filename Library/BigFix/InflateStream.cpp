#include "InflateStream.h"
#include "BigFix/DataRef.h"
#include "BigFix/Error.h"
#include <algorithm>
#include <string.h>

namespace BigFix
{

InflateStream::InflateStream( Stream& output )
  : m_output( output )
  , m_state( STATE_HEADER )
  , m_zstatus( Z_OK )
  , m_pos( 0 )
{
  memset( &m_zstream, 0, sizeof( m_zstream ) );

  if ( inflateInit( &m_zstream ) )
    throw Error( "Failed to initialize zlib" );
}

InflateStream::~InflateStream()
{
  inflateEnd( &m_zstream );
}

void InflateStream::Write( DataRef data )
{
  const uint8_t* start = data.Start();
  const uint8_t* end = data.End();

  while ( start != end )
  {
    switch ( m_state )
    {
    case STATE_HEADER:
      start = Header( start, end );
      break;

    case STATE_CHECKSUM:
      start = Checksum( start, end );
      break;

    case STATE_COMPRESSED:
      start = Compressed( start, end );
      break;

    case STATE_RAW:
      m_output.Write( DataRef( start, end ) );
      return;
    }
  }
}

void InflateStream::End()
{
  switch ( m_state )
  {
  case STATE_HEADER:
    if ( m_pos != 0 )
      m_output.Write( DataRef( m_header, m_header + m_pos ) );
    break;

  case STATE_CHECKSUM:
    throw Error( "Incomplete compressed data" );

  case STATE_COMPRESSED:
    if ( m_zstatus != Z_STREAM_END )
      throw Error( "Failed to decompress data" );
    break;

  case STATE_RAW:
    break;
  }

  m_output.End();
}

const uint8_t* InflateStream::Header( const uint8_t* start, const uint8_t* end )
{
  while ( start != end && m_pos != sizeof( m_header ) )
    m_header[m_pos++] = *start++;

  if ( m_pos == sizeof( m_header ) )
  {
    if ( memcmp( m_header, "##SC001\0", sizeof( m_header ) ) == 0 )
    {
      m_state = STATE_CHECKSUM;
      m_pos = 0;
    }
    else
    {
      m_state = STATE_RAW;
      m_output.Write( DataRef( m_header, m_header + sizeof( m_header ) ) );
    }
  }

  return start;
}

const uint8_t* InflateStream::Checksum( const uint8_t* start,
                                        const uint8_t* end )
{
  size_t amount = std::min<size_t>( end - start, 4 - m_pos );
  start += amount;
  m_pos += amount;

  if ( m_pos == 4 )
    m_state = STATE_COMPRESSED;

  return start;
}

const uint8_t* InflateStream::Compressed( const uint8_t* start,
                                          const uint8_t* end )
{
  uint8_t buffer[4096];

  m_zstream.next_in = const_cast<uint8_t*>( start );
  m_zstream.avail_in = static_cast<unsigned int>( end - start );

  do
  {
    m_zstream.next_out = buffer;
    m_zstream.avail_out = sizeof( buffer );

    m_zstatus = inflate( &m_zstream, Z_NO_FLUSH );
    if ( m_zstatus != Z_OK && m_zstatus != Z_STREAM_END &&
         m_zstatus != Z_BUF_ERROR )
    {
      throw Error( "Failed to decompress data" );
    }

    m_output.Write( DataRef( buffer, m_zstream.next_out ) );
  }
  while ( m_zstream.avail_out == 0 );

  return end;
}

}
