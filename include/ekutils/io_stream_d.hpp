#ifndef IO_STREAM_D_HEAD_GJHEWDE4433TRHB
#define IO_STREAM_D_HEAD_GJHEWDE4433TRHB

#include <ekutils/io_stream.hpp>
#include <ekutils/in_stream_d.hpp>
#include <ekutils/out_stream_d.hpp>

namespace ekutils {

class io_stream_d : virtual public io_stream, virtual public in_stream_d, virtual public out_stream_d {};

} // namespace ekutils

#endif // IO_STREAM_D_HEAD_GJHEWDE4433TRHB
