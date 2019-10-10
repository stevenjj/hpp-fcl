//
// Software License Agreement (BSD License)
//
//  Copyright (c) 2019-2020 CNRS-LAAS INRIA
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

#include <eigenpy/registration.hpp>

#include "fcl.hh"

#include <hpp/fcl/fwd.hh>
#include <hpp/fcl/shape/geometric_shapes.h>
#include <hpp/fcl/shape/convex.h>
#include <hpp/fcl/BVH/BVH_model.h>

#include "doxygen_autodoc/hpp/fcl/BVH/BVH_model.h"
#include "doxygen_autodoc/hpp/fcl/shape/geometric_shapes.h"

using namespace boost::python;

using namespace hpp::fcl;
using boost::shared_ptr;
using boost::noncopyable;

struct BVHModelBaseWrapper
{
  static Vec3f vertices (const BVHModelBase& bvh, int i)
  {
    if (i >= bvh.num_vertices) throw std::out_of_range("index is out of range");
    return bvh.vertices[i];
  }

  static Triangle tri_indices (const BVHModelBase& bvh, int i)
  {
    if (i >= bvh.num_tris) throw std::out_of_range("index is out of range");
    return bvh.tri_indices[i];
  }
};

template <typename BV>
void exposeBVHModel (const std::string& bvname)
{
  typedef BVHModel<BV> BVHModel_t;

  std::string type = "BVHModel" + bvname;
  class_ <BVHModel_t, bases<BVHModelBase>, shared_ptr<BVHModel_t> >
    (type.c_str(), doxygen::class_doc<BVHModel_t>(), init<>(doxygen::constructor_doc<BVHModel_t>()))
    ;
}

struct ConvexBaseWrapper
{
  static Vec3f points (const ConvexBase& convex, int i)
  {
    if (i >= convex.num_points) throw std::out_of_range("index is out of range");
    return convex.points[i];
  }

  static list neighbors (const ConvexBase& convex, int i)
  {
    if (i >= convex.num_points) throw std::out_of_range("index is out of range");
    list n;
    for (unsigned char j = 0; j < convex.neighbors[i].count(); ++j)
      n.append (convex.neighbors[i][j]);
    return n;
  }
};

template <typename PolygonT>
struct ConvexWrapper
{
  typedef Convex<PolygonT> Convex_t;

  static PolygonT polygons (const Convex_t& convex, int i)
  {
    if (i >= convex.num_polygons) throw std::out_of_range("index is out of range");
    return convex.polygons[i];
  }
};

void exposeShapes ()
{
  class_ <ShapeBase, bases<CollisionGeometry>, shared_ptr<ShapeBase>, noncopyable>
    ("ShapeBase", doxygen::class_doc<ShapeBase>(), no_init)
    //.def ("getObjectType", &CollisionGeometry::getObjectType)
    ;

  class_ <Box, bases<ShapeBase>, shared_ptr<Box> >
    ("Box", doxygen::class_doc<ShapeBase>(), init<>())
    .def (init<FCL_REAL,FCL_REAL,FCL_REAL>())
    .def (init<Vec3f>())
    .add_property("halfSide",
                  make_getter(&Box::halfSide, return_value_policy<return_by_value>()),
                  make_setter(&Box::halfSide, return_value_policy<return_by_value>()));
    ;

  class_ <Capsule, bases<ShapeBase>, shared_ptr<Capsule> >
    ("Capsule", doxygen::class_doc<Capsule>(), init<FCL_REAL, FCL_REAL>())
    .def_readwrite ("radius", &Capsule::radius)
    .def_readwrite ("halfLength", &Capsule::halfLength)
    ;

  class_ <Cone, bases<ShapeBase>, shared_ptr<Cone> >
    ("Cone", doxygen::class_doc<Cone>(), init<FCL_REAL, FCL_REAL>())
    .def_readwrite ("radius", &Cone::radius)
    .def_readwrite ("halfLength", &Cone::halfLength)
    ;

  class_ <ConvexBase, bases<ShapeBase>, shared_ptr<ConvexBase >, noncopyable>
    ("ConvexBase", doxygen::class_doc<ConvexBase>(), no_init)
    .def_readonly ("center", &ConvexBase::center)
    .def_readonly ("num_points", &ConvexBase::num_points)
    .def ("points", &ConvexBaseWrapper::points)
    .def ("neighbors", &ConvexBaseWrapper::neighbors)
    ;

  class_ <Convex<Triangle>, bases<ConvexBase>, shared_ptr<Convex<Triangle> >, noncopyable>
    ("Convex", doxygen::class_doc< Convex<Triangle> >(), no_init)
    .def_readonly ("num_polygons", &Convex<Triangle>::num_polygons)
    .def ("polygons", &ConvexWrapper<Triangle>::polygons)
    ;

  class_ <Cylinder, bases<ShapeBase>, shared_ptr<Cylinder> >
    ("Cylinder", doxygen::class_doc<Cylinder>(), init<FCL_REAL, FCL_REAL>())
    .def_readwrite ("radius", &Cylinder::radius)
    .def_readwrite ("halfLength", &Cylinder::halfLength)
    ;

  class_ <Halfspace, bases<ShapeBase>, shared_ptr<Halfspace> >
    ("Halfspace", doxygen::class_doc<Halfspace>(), init<const Vec3f&, FCL_REAL>())
    .def (init<FCL_REAL,FCL_REAL,FCL_REAL,FCL_REAL>())
    .def (init<>())
    .def_readwrite ("n", &Halfspace::n)
    .def_readwrite ("d", &Halfspace::d)
    ;

  class_ <Plane, bases<ShapeBase>, shared_ptr<Plane> >
    ("Plane", doxygen::class_doc<Plane>(), init<const Vec3f&, FCL_REAL>())
    .def (init<FCL_REAL,FCL_REAL,FCL_REAL,FCL_REAL>())
    .def (init<>())
    .def_readwrite ("n", &Plane::n)
    .def_readwrite ("d", &Plane::d)
    ;

  class_ <Sphere, bases<ShapeBase>, shared_ptr<Sphere> >
    ("Sphere", doxygen::class_doc<Sphere>(), init<FCL_REAL>())
    .def_readwrite ("radius", &Sphere::radius)
    ;

  class_ <TriangleP, bases<ShapeBase>, shared_ptr<TriangleP> >
    ("TriangleP", doxygen::class_doc<TriangleP>(), init<const Vec3f&, const Vec3f&, const Vec3f&>())
    .def_readwrite ("a", &TriangleP::a)
    .def_readwrite ("b", &TriangleP::b)
    .def_readwrite ("c", &TriangleP::c)
    ;

}

boost::python::tuple AABB_distance_proxy(const AABB & self, const AABB & other)
{
  Vec3f P,Q;
  FCL_REAL distance = self.distance(other,&P,&Q);
  return boost::python::tuple((distance,P,Q));
}

void exposeCollisionGeometries ()
{
  
  if(!eigenpy::register_symbolic_link_to_registered_type<OBJECT_TYPE>())
  {
    enum_<OBJECT_TYPE>("OBJECT_TYPE")
      .value ("OT_UNKNOWN", OT_UNKNOWN)
      .value ("OT_BVH"    , OT_BVH)
      .value ("OT_GEOM"   , OT_GEOM)
      .value ("OT_OCTREE" , OT_OCTREE)
      ;
  }
  
  if(!eigenpy::register_symbolic_link_to_registered_type<NODE_TYPE>())
  {
    enum_<NODE_TYPE>("NODE_TYPE")
      .value ("BV_UNKNOWN", BV_UNKNOWN)
      .value ("BV_AABB"  , BV_AABB)
      .value ("BV_OBB"   , BV_OBB)
      .value ("BV_RSS"   , BV_RSS)
      .value ("BV_kIOS"  , BV_kIOS)
      .value ("BV_OBBRSS", BV_OBBRSS)
      .value ("BV_KDOP16", BV_KDOP16)
      .value ("BV_KDOP18", BV_KDOP18)
      .value ("BV_KDOP24", BV_KDOP24)
      .value ("GEOM_BOX"      , GEOM_BOX)
      .value ("GEOM_SPHERE"   , GEOM_SPHERE)
      .value ("GEOM_CAPSULE"  , GEOM_CAPSULE)
      .value ("GEOM_CONE"     , GEOM_CONE)
      .value ("GEOM_CYLINDER" , GEOM_CYLINDER)
      .value ("GEOM_CONVEX"   , GEOM_CONVEX)
      .value ("GEOM_PLANE"    , GEOM_PLANE)
      .value ("GEOM_HALFSPACE", GEOM_HALFSPACE)
      .value ("GEOM_TRIANGLE" , GEOM_TRIANGLE)
      .value ("GEOM_OCTREE"   , GEOM_OCTREE)
      ;
  }
  
  namespace bp = boost::python;
  
  class_<AABB>("AABB",
               "A class describing the AABB collision structure, which is a box in 3D space determined by two diagonal points",
               no_init)
    .def(init<>("Default constructor"))
    .def(init<Vec3f>(bp::arg("v"),"Creating an AABB at position v with zero size."))
    .def(init<Vec3f,Vec3f>(bp::args("a","b"),"Creating an AABB with two endpoints a and b."))
    .def(init<AABB,Vec3f>(bp::args("core","delta"),"Creating an AABB centered as core and is of half-dimension delta."))
    .def(init<Vec3f,Vec3f,Vec3f>(bp::args("a","b","c"),"Creating an AABB contains three points."))
  
    .def("contain",
         (bool (AABB::*)(const Vec3f &) const)&AABB::contain,
         bp::args("self","p"),
         "Check whether the AABB contains a point p.")
    .def("contain",
         (bool (AABB::*)(const AABB &) const)&AABB::contain,
         bp::args("self","other"),
         "Check whether the AABB contains another AABB.")
  
    .def("overlap",
         (bool (AABB::*)(const AABB &) const)&AABB::overlap,
         bp::args("self","other"),
         "Check whether two AABB are overlap.")
    .def("overlap",
         (bool (AABB::*)(const AABB&, AABB&) const)&AABB::overlap,
         bp::args("self","other","overlapping_part"),
         "Check whether two AABB are overlaping and return the overloaping part if true.")
  
    .def("distance",
         (FCL_REAL (AABB::*)(const AABB &) const)&AABB::distance,
         bp::args("self","other"),
         "Distance between two AABBs.")
//    .def("distance",
//         AABB_distance_proxy,
//         bp::args("self","other"),
//         "Distance between two AABBs.")
  
    .def(bp::self + bp::self)
    .def(bp::self += bp::self)
    .def(bp::self += Vec3f())
  
    .def("size",&AABB::volume,bp::arg("self"),"Size of the AABB.")
    .def("center",&AABB::center,bp::arg("self"),"Center of the AABB.")
    .def("width",&AABB::width,bp::arg("self"),"Width of the AABB.")
    .def("height",&AABB::height,bp::arg("self"),"Height of the AABB.")
    .def("depth",&AABB::depth,bp::arg("self"),"Depth of the AABB.")
    .def("volume",&AABB::volume,bp::arg("self"),"Volume of the AABB.")
  
    .def("expand",
         (AABB& (AABB::*)(const AABB &, FCL_REAL))&AABB::expand,
         bp::args("self","core","ratio"),
         "Expand the AABB by increase the thickness of the plate by a ratio.",
         bp::return_internal_reference<>())
    .def("expand",
         (AABB& (AABB::*)(const Vec3f &))&AABB::expand,
         bp::args("self","delta"),
         "Expand the half size of the AABB by delta, and keep the center unchanged.",
         bp::return_internal_reference<>());
  
  def("translate",
      (AABB (*)(const AABB&, const Vec3f&))&translate,
      bp::args("aabb","t"),
      "Translate the center of AABB by t");
  
  def("rotate",
      (AABB (*)(const AABB&, const Matrix3f&))&rotate,
      bp::args("aabb","R"),
      "Rotate the AABB object by R");

  if(!eigenpy::register_symbolic_link_to_registered_type<CollisionGeometry>())
  {
    class_ <CollisionGeometry, CollisionGeometryPtr_t, noncopyable>
      ("CollisionGeometry", no_init)
      .def ("getObjectType", &CollisionGeometry::getObjectType)
      .def ("getNodeType", &CollisionGeometry::getNodeType)

      .def ("computeLocalAABB", &CollisionGeometry::computeLocalAABB)

      .def ("computeCOM", &CollisionGeometry::computeCOM)
      .def ("computeMomentofInertia", &CollisionGeometry::computeMomentofInertia)
      .def ("computeVolume", &CollisionGeometry::computeVolume)
      .def ("computeMomentofInertiaRelatedToCOM", &CollisionGeometry::computeMomentofInertiaRelatedToCOM)

      .def_readwrite("aabb_radius",&CollisionGeometry::aabb_radius,"AABB radius.")
      .def_readwrite("aabb_center",&CollisionGeometry::aabb_center,"AABB center in local coordinate.")
      .def_readwrite("aabb_local",&CollisionGeometry::aabb_local,"AABB in local coordinate, used for tight AABB when only translation transform.")
    
      .def("isOccupied",&CollisionGeometry::isOccupied,bp::arg("self"),"Whether the object is completely occupied.")
      .def("isFree",&CollisionGeometry::isFree,bp::arg("self"),"Whether the object is completely free.")
      .def("isUncertain",&CollisionGeometry::isUncertain,bp::arg("self"),"Whether the object has some uncertainty.")

      .def_readwrite("cost_density",&CollisionGeometry::cost_density,"Collision cost for unit volume.")
      .def_readwrite("threshold_occupied",&CollisionGeometry::threshold_occupied,"Threshold for occupied ( >= is occupied).")
      .def_readwrite("threshold_free",&CollisionGeometry::threshold_free,"Threshold for free (<= is free).")
      ;
  }

  exposeShapes();

  class_ <BVHModelBase, bases<CollisionGeometry>, BVHModelPtr_t, noncopyable>
    ("BVHModelBase", no_init)
    .def ("vertices", &BVHModelBaseWrapper::vertices,
          bp::args("self","index"),"Retrieve the vertex given by its index.")
    .def ("tri_indices", &BVHModelBaseWrapper::tri_indices,
          bp::args("self","index"),"Retrieve the triangle given by its index.")
    .def_readonly ("num_vertices", &BVHModelBase::num_vertices)
    .def_readonly ("num_tris", &BVHModelBase::num_tris)

    .def_readonly ("convex", &BVHModelBase::convex)

    .def ("buildConvexRepresentation", &BVHModelBase::buildConvexRepresentation)
    ;
  exposeBVHModel<OBB    >("OBB"    );
  exposeBVHModel<OBBRSS >("OBBRSS" );
}
