#ifndef _LOADOBJ_HPP
#define _LOADOBJ_HPP

#include <memory>
#include <VertexArray.hpp>

std::unique_ptr<VertexArray> load_obj(const char * filename);

#endif /* _LOADOBJ_HPP */
