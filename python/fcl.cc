//
// Software License Agreement (BSD License)
//
//  Copyright (c) 2019 CNRS-LAAS
//  Author: Joseph Mirabel
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above
//     copyright notice, this list of conditions and the following
//     disclaimer in the documentation and/or other materials provided
//     with the distribution.
//   * Neither the name of CNRS-LAAS. nor the names of its
//     contributors may be used to endorse or promote products derived
//     from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.

#include <boost/python.hpp>

#include "fcl.hh"

#include <hpp/fcl/fwd.hh>
#include <hpp/fcl/shape/geometric_shapes.h>
#include <hpp/fcl/BVH/BVH_model.h>

#include <hpp/fcl/mesh_loader/loader.h>

#include <hpp/fcl/collision.h>

using namespace boost::python;

using namespace hpp::fcl;
using boost::shared_ptr;
using boost::noncopyable;

void exposeMeshLoader ()
{
  class_ <MeshLoader> ("MeshLoader", init< optional< NODE_TYPE> >())
    .def ("load", static_cast <BVHModelPtr_t (MeshLoader::*) (const std::string&, const Vec3f&)> (&MeshLoader::load))
    ;

  class_ <CachedMeshLoader, bases<MeshLoader> > ("CachedMeshLoader", init< optional< NODE_TYPE> >())
    ;
}

BOOST_PYTHON_MODULE(hppfcl)
{
  exposeVersion();
  exposeMaths();
  exposeCollisionGeometries();
  exposeMeshLoader();
  exposeCollisionAPI();
  exposeDistanceAPI();
}
