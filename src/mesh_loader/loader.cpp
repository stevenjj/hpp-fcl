/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2011-2014, Willow Garage, Inc.
 *  Copyright (c) 2014-2015, Open Source Robotics Foundation
 *  Copyright (c) 2016, CNRS - LAAS
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Open Source Robotics Foundation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include <hpp/fcl/mesh_loader/loader.h>
#include <hpp/fcl/mesh_loader/assimp.h>

#include <hpp/fcl/BV/BV.h>

namespace fcl {
  bool operator< (const CachedMeshLoader::Key& a, const CachedMeshLoader::Key& b)
  {
    if (a.bvType < b.bvType) return true;
    if (a.bvType > b.bvType) return false;
    for (int i = 0; i < 3; ++i) {
      if (a.scale[i] < b.scale[i]) return true;
      else if (a.scale[i] > b.scale[i]) return false;
    }
    return std::less<std::string>() (a.filename, b.filename);
  }

  template <typename BV>
  CollisionGeometryPtr_t _load (const std::string& filename, const Vec3f& scale)
  {
    boost::shared_ptr < BVHModel<BV> > polyhedron (new BVHModel<BV>);
    loadPolyhedronFromResource (filename, scale, polyhedron);
    return polyhedron;
  }

  CollisionGeometryPtr_t MeshLoader::load (const std::string& filename,
      const Vec3f& scale,
      const NODE_TYPE& bvType)
  {
    switch (bvType) {
      case BV_AABB  : return _load <AABB  > (filename, scale);
      case BV_OBB   : return _load <OBB   > (filename, scale);
      case BV_RSS   : return _load <RSS   > (filename, scale);
      case BV_kIOS  : return _load <kIOS  > (filename, scale);
      case BV_OBBRSS: return _load <OBBRSS> (filename, scale);
      case BV_KDOP16: return _load <KDOP<16> > (filename, scale);
      case BV_KDOP18: return _load <KDOP<18> > (filename, scale);
      case BV_KDOP24: return _load <KDOP<24> > (filename, scale);
      default:
        throw std::invalid_argument("Unhandled bouding volume type.");
    }
  }

  CollisionGeometryPtr_t CachedMeshLoader::load (const std::string& filename,
      const Vec3f& scale,
      const NODE_TYPE& bvType)
  {
    Key key (filename, scale, bvType);
    Cache_t::const_iterator _cached = cache_.find (key);
    if (_cached == cache_.end()) {
      CollisionGeometryPtr_t geom = MeshLoader::load (filename, scale, bvType);
      cache_.insert (std::make_pair(key, geom));
      return geom;
    } else {
      return _cached->second;
    }
  }
}
