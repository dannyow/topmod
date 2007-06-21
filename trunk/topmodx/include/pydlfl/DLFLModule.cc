#include <Python.h>

#include <DLFLCore.hh>
#include <DLFLExtrude.hh>
#include <DLFLSubdiv.hh>
#include <DLFLDual.hh>

/// Prototypes

/* Object Management */
static PyObject *dlfl_load_obj(PyObject *self, PyObject *args);
static PyObject *dlfl_kill_obj(PyObject *self, PyObject *args);
static PyObject *dlfl_switch_obj(PyObject *self, PyObject *args);
static PyObject *dlfl_save_obj(PyObject *self, PyObject *args);
//static PyObject *dlfl_query_obj(PyObject *self, PyObject *args);

/* Core */
static PyObject *dlfl_insert_edge(PyObject *self, PyObject *args);
static PyObject *dlfl_delete_edge(PyObject *self, PyObject *args);
static PyObject *dlfl_collapse_edge(PyObject *self, PyObject *args);
static PyObject *dlfl_subdivide_edge(PyObject *self, PyObject *args);

static PyObject *dlfl_create_vertex(PyObject *self, PyObject *args);
static PyObject *dlfl_remove_vertex(PyObject *self, PyObject *args);

/* IDs */
static PyObject *dlfl_faces(PyObject *self, PyObject *args);
static PyObject *dlfl_edges(PyObject *self, PyObject *args);
static PyObject *dlfl_verts(PyObject *self, PyObject *args);
static PyObject *dlfl_faceverts(PyObject *self, PyObject *args);

static PyObject *dlfl_boundary_walk(PyObject *self, PyObject *args);
static PyObject *dlfl_walk_vertices(PyObject *self, PyObject *args);
static PyObject *dlfl_walk_edges(PyObject *self, PyObject *args);

/* Info */
static PyObject *dlfl_print_object(PyObject *self, PyObject *args);
static PyObject *dlfl_vertexInfo(PyObject *self, PyObject *args);
static PyObject *dlfl_edgeInfo(PyObject *self, PyObject *args);
static PyObject *dlfl_faceInfo(PyObject *self, PyObject *args);
static PyObject *dlfl_centroid(PyObject *self, PyObject *args);

/* Auxiliary */
static PyObject *dlfl_extrude(PyObject *self, PyObject *args);
static PyObject *dlfl_subdivide(PyObject *self, PyObject *args);
static PyObject *dlfl_subdiv_face(PyObject *self, PyObject *args);
static PyObject *dlfl_dual(PyObject *self, PyObject *args);

/* Transform */
static PyObject *dlfl_translate(PyObject *self, PyObject *args);
static PyObject *dlfl_scale(PyObject *self, PyObject *args);
static PyObject *dlfl_move(PyObject *self, PyObject *args);

PyMODINIT_FUNC initdlfl(void);

static PyMethodDef DLFLMethods[] = {
	/* Object Management */
  {"load",           dlfl_load_obj,       METH_VARARGS, "load(string)"},
  {"save",           dlfl_save_obj,       METH_VARARGS, "save(string)"},  
  {"kill",           dlfl_kill_obj,       METH_VARARGS, "kill(object id)"},
  {"switch",         dlfl_switch_obj,     METH_VARARGS, "switch(object id)"},
  //{"query",          dlfl_query_obj,      METH_VARARGS, "query()"},
	/* Core */
  {"insertEdge",     dlfl_insert_edge,    METH_VARARGS, "insert_edge((face,vert),(face,vert))"},
  {"deleteEdge",     dlfl_delete_edge,    METH_VARARGS, "Delete an Edge."},
  {"collapseEdge",   dlfl_collapse_edge,  METH_VARARGS, "Collapse an Edge."},
  {"subdivideEdge",  dlfl_subdivide_edge, METH_VARARGS, "Subdivide an Edge."},
	{"createVertex",   dlfl_create_vertex,  METH_VARARGS, "Create a vertex at (x,y,z)"},
	{"removeVertex",   dlfl_remove_vertex,  METH_VARARGS, "Remove a vertex by ID"},
	/* IDs */
  {"faces",          dlfl_faces,          METH_VARARGS, "Grab all/selected the faces of the object"},
  {"edges",          dlfl_edges,          METH_VARARGS, "Grab all/selected the edges of the object"},
  {"verts",          dlfl_verts,          METH_VARARGS, "Grab all/selected the vertices of the object"},
  {"faceverts",      dlfl_faceverts,      METH_VARARGS, "Grab all/selected the face-vertices of the object"},
  {"walk",           dlfl_boundary_walk,  METH_VARARGS, "walk(int)"},
  {"walkVertices",   dlfl_walk_vertices,  METH_VARARGS, "walk_vertices(int)"},
  {"walkEdges",      dlfl_walk_edges,     METH_VARARGS, "walk_edges(int)"},
	/* Info */
  {"printObject",   dlfl_print_object,   METH_VARARGS, "Print object information"},
  {"vertexInfo",    dlfl_vertexInfo,     METH_VARARGS, "Position,Valence,etc into dictionary"},
  {"edgeInfo",      dlfl_edgeInfo,       METH_VARARGS, "Midpoint,Verts,etc into dictionary"},
  {"faceInfo",      dlfl_faceInfo,       METH_VARARGS, "Centroid,Verts,etc into dictionary"},
  {"centroid",      dlfl_centroid,       METH_VARARGS, "Get centroid of vertices"},
  /* Auxiliary Below */
  {"extrude",        dlfl_extrude,        METH_VARARGS, "Extrude a face"},
  {"subdivide",      dlfl_subdivide,      METH_VARARGS, "Subdivide a mesh"},
  {"subdivFace",     dlfl_subdiv_face,    METH_VARARGS, "Subdivide a Face"},
  {"dual",           dlfl_dual,           METH_VARARGS, "Dual of mesh"},
	/* Transform */
  {"translate",      dlfl_translate,      METH_VARARGS, "Translate Object"},
  {"scale",          dlfl_scale,          METH_VARARGS, "Scale Object"},
  {"move",      dlfl_move,      METH_VARARGS, "Move Vertices"},
  {NULL, NULL, 0, NULL}        /* Sentinel */
};

static PyObject *DLFLError;
static bool usingGUI = false;

// Pointer to the current object that we are editing
DLFL::DLFLObject *currObj = 0;
static DLFL::DLFLObjectPtrArray objArray;

/**
 *  Load/Save Files
 */

static PyObject *
dlfl_load_obj(PyObject *self, PyObject *args)
{
  char *filename;
  int size;
  int objId = -1;

  if( !PyArg_ParseTuple(args, "s#", &filename, &size) )
    return NULL;

  if( !usingGUI ) {
    currObj = DLFL::readObjectFile( filename );
    objArray.push_back( currObj );

    if( currObj )
      objId = objArray.size()-1; //currObj->getID();
  } else {
    objId = -2;
  }

  return Py_BuildValue("i", objId );
}
/*
static PyObject *
dlfl_query_obj(PyObject *self, PyObject *args)
{
  return Py_BuildValue("i", usingGUI );
}
*/
static PyObject *
dlfl_kill_obj(PyObject *self, PyObject *args)
{
  int idx;
  if( !PyArg_ParseTuple(args, "i", &idx) )
    return NULL;

  DLFL::DLFLObjectPtrArray::iterator it = objArray.begin();
  for( int i = 0; i < idx; i++) { it++; }
  DLFL::DLFLObjectPtr obj = objArray[idx];

  /*if( currObj == obj ) {
    if( objArray.size() > 1 )
    currObj = objArray[objArray.size()-1];
    else
    }*/

  objArray.erase( it );
  currObj = NULL;

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
dlfl_switch_obj(PyObject *self, PyObject *args)
{
  int idx;
  if( !PyArg_ParseTuple(args, "i", &idx) )
    return NULL;
  
 int objId = idx;
  if( idx < 0 || idx >= objArray.size() ) {
    objId = -1;
  } else {
    currObj = objArray[idx];
  }
  return Py_BuildValue("i", objId );
}

static PyObject *
dlfl_save_obj(PyObject *self, PyObject *args)
{
  char *filename; int size;
  if( !PyArg_ParseTuple(args, "s#", &filename, &size) )
    filename = NULL;

  bool wrote;
  if( currObj )
    wrote = DLFL::writeObjectFile( currObj, filename );
  else
    wrote = false;

  return Py_BuildValue( "b", wrote );
}

/**
 * Insert/Delete/Collapse/Subdivide Edge
 */

static PyObject *
dlfl_insert_edge(PyObject *self, PyObject *args)
{
  int faceId1;
  int vertId1;
  int faceId2;
  int vertId2;
  int edgeId = -1;

  if( !PyArg_ParseTuple(args, "(ii)(ii)", &faceId1, &vertId1, &faceId2, &vertId2) )
    return NULL;

  if( currObj ) {
    edgeId = DLFL::insertEdge( currObj, faceId1, vertId1, faceId2, vertId2 );
    currObj->clearSelected( );
  }

  return Py_BuildValue("i", edgeId );
}

static PyObject *
dlfl_delete_edge(PyObject *self, PyObject *args)
{
  int edgeId;

  if( !PyArg_ParseTuple(args, "i", &edgeId) )
    return NULL;
  if( !currObj )
    return NULL;

  if( currObj ) {
    DLFL::deleteEdgeID( currObj, edgeId );
    currObj->clearSelected( );
  }

  return Py_BuildValue("i", edgeId );
}

static PyObject *
dlfl_collapse_edge(PyObject *self, PyObject *args)
{
  int edgeId;

  if( !PyArg_ParseTuple(args, "i", &edgeId) )
    return NULL;
  if( !currObj )
    return NULL;

  int vertId = DLFL::collapseEdgeID( currObj, edgeId );
  currObj->clearSelected( );

  return Py_BuildValue("i", vertId );
}

static PyObject *
dlfl_subdivide_edge(PyObject *self, PyObject *args)
{
  int edgeId, numDivs;

  if( !currObj )
    return NULL;

  if( PyArg_ParseTuple(args, "i", &edgeId) ) {
    int vertId = DLFL::subdivideEdgeID( currObj, edgeId );
    currObj->clearSelected( );
    return Py_BuildValue("i", vertId );
  } else if( PyArg_ParseTuple(args, "ii", &numDivs, &edgeId ) ) {
    vector<int> verts = DLFL::subdivideEdgeID( currObj, numDivs, edgeId );
    PyObject *vert;
    PyObject *list = PyList_New(verts.size());
    for( int i = 0; i < verts.size(); i++) {
      vert = Py_BuildValue("i", verts[i]);
      PyList_SetItem(list, i, vert);
    }
    currObj->clearSelected( );
    Py_INCREF(list);
    return list;
  }

  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject *dlfl_create_vertex(PyObject *self, PyObject *args) {
	double x, y, z;

	if( !PyArg_ParseTuple(args, "(ddd)", &x, &y, &z) )
			return NULL;

	uint *fvid = DLFL::createVertex( x, y, z, currObj );
	uint fid = fvid[0];
	uint vid = fvid[1];
	delete fvid; fvid = 0;
  return Py_BuildValue("(ii)", fid, vid );
}

static PyObject *dlfl_remove_vertex(PyObject *self, PyObject *args) {
	uint vid;
	if( !PyArg_ParseTuple(args, "i", &vid) )
		return NULL;
	DLFL::removeVertex( currObj, vid );

  Py_INCREF(Py_None);
  return Py_None;
}

/* Mesh Identification Helpers (Verts,Edges,Faces, FaceVertices ) */

static PyObject *
dlfl_faces(PyObject *self, PyObject *args)
{
  if( !currObj ) {
    Py_INCREF(Py_None);
    return Py_None;
  }

  bool selected = false;
  if( usingGUI ) {
    selected = true;
    if( !PyArg_ParseTuple(args, "|b", &selected ) )
      return NULL;
  }

  PyObject *flist=0, *face=0;
  if(selected) {
    DLFL::DLFLFacePtrArray fpa = currObj->sel_fptr_array;
    flist = PyList_New(fpa.size());
    for( int i = 0; i < (int)fpa.size(); i++ ) {
      face = Py_BuildValue("i", fpa[i]->getID());
      PyList_SetItem(flist, i, face);
    }
  } else {
    DLFL::DLFLFacePtrList fpl = currObj->getFaceList();
    DLFL::DLFLFacePtrList::iterator it; int i;
    flist = PyList_New(fpl.size());
    for( i = 0, it = fpl.begin(); it != fpl.end(); i++, it++ ) {
      face = Py_BuildValue("i", (*it)->getID());
      PyList_SetItem(flist, i, face);
    }
  }
  Py_INCREF(flist);
  return flist;
}

static PyObject *
dlfl_edges(PyObject *self, PyObject *args)
{
  if( !currObj ) {
    Py_INCREF(Py_None);
    return Py_None;
  }

  bool selected = false;
  if( usingGUI ) {
    selected = true;
    if( !PyArg_ParseTuple(args, "|b", &selected ) )
      return NULL;
  }

  PyObject *elist=0, *edge=0;
  if(selected) {
    DLFL::DLFLEdgePtrArray epa = currObj->sel_eptr_array;
    elist = PyList_New(epa.size());
    for( int i = 0; i < (int)epa.size(); i++ ) {
      edge = Py_BuildValue("i", epa[i]->getID());
      PyList_SetItem(elist, i, edge);
    }
  } else {
    DLFL::DLFLEdgePtrList epl = currObj->getEdgeList();
    DLFL::DLFLEdgePtrList::iterator it; int i;
    elist = PyList_New(epl.size());
    for( i = 0, it = epl.begin(); it != epl.end(); i++, it++ ) {
      edge = Py_BuildValue("i", (*it)->getID());
      PyList_SetItem(elist, i, edge);
    }
  }
  Py_INCREF(elist);
  return elist;
}

static PyObject *
dlfl_verts(PyObject *self, PyObject *args)
{
  if( !currObj ) {
    Py_INCREF(Py_None);
    return Py_None;
  }

  bool selected = false;
  if( usingGUI ) {
    selected = true;
    if( !PyArg_ParseTuple(args, "|b", &selected ) )
      return NULL;
  }

  PyObject *vlist=0, *vert=0;
  if(selected) {
    DLFL::DLFLVertexPtrArray vpa = currObj->sel_vptr_array;
    vlist = PyList_New(vpa.size());
    for( int i = 0; i < (int)vpa.size(); i++ ) {
      vert = Py_BuildValue("i", vpa[i]->getID());
      PyList_SetItem(vlist, i, vert);
    }
  } else {
    DLFL::DLFLVertexPtrList vpl = currObj->getVertexList();
    DLFL::DLFLVertexPtrList::iterator it; int i;
    vlist = PyList_New(vpl.size());
    for( i = 0, it = vpl.begin(); it != vpl.end(); i++, it++ ) {
      vert = Py_BuildValue("i", (*it)->getID());
      PyList_SetItem(vlist, i, vert);
    }
  }
  Py_INCREF(vlist);
  return vlist;  
}

static PyObject *
dlfl_faceverts(PyObject *self, PyObject *args)
{
  if( !currObj ) {
    Py_INCREF(Py_None);
    return Py_None;
  }

  bool selected = false;
  if( usingGUI ) {
    selected = true;
    if( !PyArg_ParseTuple(args, "|b", &selected ) )
      return NULL;
  }

  PyObject *fvlist=0, *fvert=0;
  if(selected) {
    DLFL::DLFLFaceVertexPtrArray fvpa = currObj->sel_fvptr_array;
    fvlist = PyList_New(fvpa.size());
    for( int i = 0; i < (int)fvpa.size(); i++ ) {
      fvert = Py_BuildValue("(ii)", fvpa[i]->getFaceID(), fvpa[i]->getVertexID() );
      PyList_SetItem(fvlist, i, fvert);
    }
  } else {
    DLFL::DLFLFaceVertexPtrArray fvpa;
    DLFL::DLFLFacePtrList fpl = currObj->getFaceList();
    DLFL::DLFLFacePtrList::iterator it; int i;
    for( i = 0, it = fpl.begin(); it != fpl.end(); i++, it++ ) {
      DLFL::DLFLFaceVertexPtr head = (*it)->front();
      DLFL::DLFLFaceVertexPtr curr = head;
      do {
	fvpa.push_back(curr);
	DLFL::advance(curr);
      } while(curr != head);
    }
    fvlist = PyList_New(fvpa.size());
    for( int i = 0; i < (int)fvpa.size(); i++ ) {
      fvert = Py_BuildValue("(ii)", fvpa[i]->getFaceID(), fvpa[i]->getVertexID() );
      PyList_SetItem(fvlist, i, fvert);
    }
  }
  Py_INCREF(fvlist);
  return fvlist;
}

static PyObject *
dlfl_boundary_walk(PyObject *self, PyObject *args) {
  int faceId;

  if( !currObj )
    return NULL;
  if( !PyArg_ParseTuple(args, "i", &faceId) )
    return NULL;
  currObj->boundaryWalkID( faceId );

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *dlfl_walk_vertices(PyObject *self, PyObject *args) {
  int faceId;

  vector<int> verts; // output: edges walked
  PyObject *list; // py output: edges walked
  PyObject *vert; // for looping

  if( !currObj )
    return NULL;

  if( !PyArg_ParseTuple(args, "i", &faceId) )
    return NULL;
  verts = currObj->vertWalk( faceId );

  list = PyList_New(verts.size());
  for( int i = 0; i < verts.size(); i++) {
    vert = Py_BuildValue("i", verts[i]);
    PyList_SetItem(list, i, vert);
  }
  Py_INCREF(list);
  return list;
}

static PyObject *dlfl_walk_edges(PyObject *self, PyObject *args) {
  int faceId; // input: face to walk

  vector<int> edges; // output: edges walked
  PyObject *list; // py output: edges walked
  PyObject *edge; // for looping
  
  if( !currObj )
    return NULL;

  if( !PyArg_ParseTuple(args, "i", &faceId) )
    return NULL;
  edges = currObj->edgeWalk( faceId );

  list = PyList_New(edges.size());
  for( int i = 0; i < edges.size(); i++) {
    edge = Py_BuildValue("i", edges[i]);
    PyList_SetItem(list, i, edge);
  }
  Py_INCREF(list);
  return list;
}

static PyObject *
dlfl_print_object(PyObject *self, PyObject *args) {
  if( !currObj )
    return NULL;

  currObj->print();

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *dlfl_vertexInfo(PyObject *self, PyObject *args) {
	if( !currObj ) {
		Py_INCREF(Py_None);
		return Py_None;
	} else {
		int id;
		if( !PyArg_ParseTuple(args, "i", &id) )
			return NULL;
		DLFL::DLFLVertexPtr vp = currObj->findVertex(id);
		const double *coords = (vp->getCoords()).getCArray();
		int valence = vp->valence();
		return Py_BuildValue("{s:i,s:(ddd),s:i}", 
												 "id", id, 
												 "coords", coords[0], coords[1], coords[2],
												 "valence", valence);
	}
}

static PyObject *dlfl_edgeInfo(PyObject *self, PyObject *args) { 
	if( !currObj ) {
		Py_INCREF(Py_None);
		return Py_None;
	} else {
		int id;
		if( !PyArg_ParseTuple(args, "i", &id) )
			return NULL;
		DLFL::DLFLEdgePtr ep = currObj->findEdge(id);
		const double *midpoint = (ep->getMidPoint()).getCArray();
		const double *normal = (ep->getNormal()).getCArray();

		DLFL::DLFLFaceVertexPtr fvpa;
		DLFL::DLFLFaceVertexPtr fvpb;
		ep->getFaceVertexPointers( fvpa, fvpb );
		int fa = fvpa->getFaceID();
		int va = fvpa->getVertexID();
		int fb = fvpb->getFaceID();
		int vb = fvpa->getVertexID();

		double length = ep->length();

		return Py_BuildValue("{s:i,s:(ddd),s:(ddd),s:(ii),s:(ii),s:d}", 
												 "id", id, 
												 "midpoint", midpoint[0], midpoint[1], midpoint[2],
												 "normal", normal[0], normal[1], normal[2],
												 "fvA", fa, va,
												 "fvB", fb, vb,
												 "length", length );
	}
}

static PyObject *dlfl_faceInfo(PyObject *self, PyObject *args) { 
	if( !currObj ) {
		Py_INCREF(Py_None);
		return Py_None;
	} else {
		int id;
		if( !PyArg_ParseTuple(args, "i", &id) )
			return NULL;
		DLFL::DLFLFacePtr fp = currObj->findFace(id);
		const double *centroid = (fp->geomCentroid()).getCArray();
		const double *normal = (fp->getNormal(true)).getCArray();
		return Py_BuildValue("{s:i,s:(ddd),s:(ddd),s:i}", 
												 "id", id, 
												 "centroid", centroid[0], centroid[1], centroid[2],
												 "normal", normal[0], normal[1], normal[2],
												 "size", fp->size());
	}
}

static PyObject *dlfl_centroid(PyObject *self, PyObject *args) { 
	if( !currObj ) {
		Py_INCREF(Py_None);
		return Py_None;
	} else {
		int id;
		if( !PyArg_ParseTuple(args, "i", &id) )
			return NULL;
		DLFL::DLFLVertexPtr vp = currObj->findVertex(id);
		const double *coords = (vp->getCoords()).getCArray();
		int valence = vp->valence();
		return Py_BuildValue("{s:i,s:(d,d,d),s:i}", 
												 "id", id, 
												 "coords", coords[0], coords[1], coords[2],
												 "valence", valence);
	}
}

 /* Auxiliary */

static PyObject *
dlfl_extrude(PyObject *self, PyObject *args) {
  if( !currObj ) {
    Py_INCREF(Py_None);
    return Py_None;
  }

  const char* choices[] = { "doo-sabin",
			    "dodeca",
			    "icosa",
			    "octa",
			    "stellate",
			    "double-stellate",
			    "cubical" };

  char* extrudeType;
  int size, faceid;
  double dist = 2.0; int num = 1; double rotTwist = 0.0; double sf = 1.0; bool boolean = false;
  if( !PyArg_ParseTuple(args, "s#i|diddb", &extrudeType, &size, &faceid, 
			&dist, &num, &rotTwist, &sf, &boolean ) )
    return NULL;

  int choice = -1;
  for(int i = 0; i < 7; i++ ) {
    if( strncmp(extrudeType,choices[i],size) == 0 ) {
      choice = i;
      break;
    }
  }

  DLFL::DLFLFacePtr fp = currObj->findFace(faceid);
  DLFL::DLFLFacePtr ofp = 0;
  int outFaceID = -1;

  if( choice != -1 ) {
    switch( choice ) {
    case 0 : // doo-sabin
      ofp = DLFL::extrudeFaceDS( currObj, fp, dist, num, rotTwist, sf );
      break;
    case 1 : // dodeca
      ofp = DLFL::extrudeFaceDodeca( currObj, fp, dist, num, rotTwist, sf, boolean );
      break;
    case 2 : // icosa
      ofp = DLFL::extrudeFaceIcosa( currObj, fp, dist, num, rotTwist, sf );
      break;
    case 3 : // octa a.k.a. extrude dual face
      ofp = DLFL::extrudeDualFace( currObj, fp, dist, num, rotTwist, sf, boolean );
      break;
    case 4 : // stellate
      DLFL::stellateFace( currObj, fp, dist );
      break;
    case 5 : // double-stellate
      DLFL::doubleStellateFace( currObj, fp, dist );
      break;
    case 6 : // cubical
    default : // cubical
      ofp = DLFL::extrudeFace( currObj, fp, dist, num, rotTwist, sf );
    }

    if( ofp )
      outFaceID = ofp->getID();

    currObj->clearSelected( );

    return Py_BuildValue("i", outFaceID );
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *
dlfl_subdivide(PyObject *self, PyObject *args) {
  if( !currObj ) {
    Py_INCREF(Py_None);
    return Py_None;
  }

  int choiceSize = 22;
  const char* choices[] = { "loop",
			    "checker",
			    "simplest",
			    "vertex-cut",
			    "pentagon",
			    "dual-pentagon",
			    "honeycomb",
			    "doo-sabin",
			    "doo-sabin-bc",
			    "doo-sabin-bc-new",
			    "corner-cut",
			    "modified-corner-cut",
			    "root4",
			    "catmull-clark",
			    "star",
			    "sqrt3",
			    "fractal",
			    "stellate",
			    "double-stellate",
			    "dome",
			    "dual-12.6.4",
			    "linear-vertex"};

  char* subdivType;
  int size;
  double attrb1 = 0, attrb2 = 0; 
  bool battrb1 = false, battrb2 = false; 
  int argc = PyArg_ParseTuple(args, "s#|dd", &subdivType, &size, &attrb1, &attrb2 );
  if( !argc )
    return NULL;
  if( argc >= 3 )
    battrb1 = true;
  if( argc >= 4)
    battrb2 = true;

  int choice = -1;
  for(int i = 0; i < choiceSize; i++ ) {
    if( strncmp(subdivType,choices[i],size) == 0 ) {
      choice = i;
      break;
    }
  }

  if( choice != -1 ) {
    switch( choice ) {
    case 0 : // loop
      DLFL::loopSubdivide( currObj );
      break;
    case 1 : // checker
      if( battrb1 ) DLFL::checkerBoardRemeshing( currObj, attrb1 );
      else DLFL::checkerBoardRemeshing( currObj );
      break;
    case 2 : // simplest
      DLFL::simplestSubdivide( currObj );
      break;
    case 3 : // vertex-cut
      if( battrb1 ) DLFL::vertexCuttingSubdivide( currObj, attrb1 );
      else DLFL::vertexCuttingSubdivide( currObj );
      break;
    case 4 : // pentagon
      DLFL::pentagonalSubdivide( currObj, attrb1 ); // since default is 0 don't need if...else
      break;
    case 5 : // dual-pentagon
      if( battrb1 ) DLFL::pentagonalSubdivide2( currObj, attrb1 );
      else DLFL::pentagonalSubdivide2( currObj );
      break;
    case 6 : // honeycomb
      DLFL::honeycombSubdivide( currObj );
      break;
    case 7 : // doo-sabin
      if( battrb1 ) DLFL::dooSabinSubdivide( currObj, (bool)attrb1 );
      else DLFL::dooSabinSubdivide( currObj );
      break;
    case 8 : // doo-sabin-bc
      if( battrb1 ) DLFL::dooSabinSubdivideBC( currObj, (bool)attrb1 );
      else DLFL::dooSabinSubdivideBC( currObj );
      break;
    case 9 : // doo-sabin-bc-new
      DLFL::dooSabinSubdivideBCNew( currObj, attrb1, attrb2 ); // no defaults
      break;
    case 10 : // corner-cut
      DLFL::cornerCuttingSubdivide( currObj );
      break;
    case 11 : // modified-corner-cut
      DLFL::modifiedCornerCuttingSubdivide( currObj, attrb1 );
      break;
    case 12 : // root4
      if( battrb1 && battrb2 ) DLFL::root4Subdivide( currObj, attrb1, attrb2 );
      else if( battrb1 ) DLFL::root4Subdivide( currObj, attrb1 );
      else DLFL::root4Subdivide( currObj );
      break;
    case 13 : // catmull-clark
      DLFL::catmullClarkSubdivide( currObj );
      break;
    case 14 : // star
      if( battrb1 ) DLFL::starSubdivide( currObj, attrb1 );
      else DLFL::starSubdivide( currObj );
      break;
    case 15 : // sqrt3
      DLFL::sqrt3Subdivide( currObj );
      break;
    case 16 : // fractal
      if( battrb1 ) DLFL::fractalSubdivide( currObj, attrb1 );
      else DLFL::fractalSubdivide( currObj );
      break;
    case 17 : // stellate
      DLFL::stellateSubdivide( currObj );
      break;
    case 18 : // double-stellate
      DLFL::twostellateSubdivide( currObj, attrb1, attrb2 ); // no defaults
      break;
    case 19 : // dome
      DLFL::domeSubdivide( currObj, attrb1, attrb2 ); // no defaults
      break;
    case 20 : // dual-12.6.4
      DLFL::dual1264Subdivide( currObj, attrb1 ); // no defaults
      break;
    case 21 : // linear-vertex
    default : // linear-vertex
      if( battrb1 ) DLFL::subdivideAllFaces( currObj, (bool)attrb1 );
      else DLFL::subdivideAllFaces( currObj );
    }
  }
  Py_INCREF(Py_None);
  return Py_None;
}


static PyObject *
dlfl_subdiv_face(PyObject *self, PyObject *args) {
  int fid;
  bool usequads = true;

  if( !PyArg_ParseTuple(args, "i|b", &fid, &usequads) )
    return NULL;

  if( currObj ) {
    DLFL::subdivideFace( currObj, currObj->findFace(fid), usequads );
    currObj->clearSelected( );
  }

  Py_INCREF(Py_None);
  return Py_None;
}

static 
PyObject *dlfl_dual(PyObject *self, PyObject *args) 
{
  bool accurate = false;
  PyArg_ParseTuple(args, "|b", &accurate);
  if( currObj ) {
    DLFL::createDual( currObj, accurate );
    currObj->clearSelected( );
  }
  Py_INCREF(Py_None);
  return Py_None;
}

static PyObject *dlfl_translate(PyObject *self, PyObject *args) { return NULL; }
static PyObject *dlfl_scale(PyObject *self, PyObject *args) { return NULL; }
static PyObject *dlfl_move(PyObject *self, PyObject *args) { return NULL; }

/* C API Stuff */
//static void **PyDLFL_API;

#define NUM_C_API_FUNCS 2

static void
PyDLFL_UsingGUI( bool ugui ) 
{ 
  usingGUI = true;
}

static void
PyDLFL_PassObject( DLFL::DLFLObject* obj )
{
  currObj = obj;
}

/**
 * Initialize
 */

PyMODINIT_FUNC initdlfl(void)
{
    PyObject *dlfl;
    static void *PyDLFL_API[NUM_C_API_FUNCS];

    dlfl = Py_InitModule("dlfl", DLFLMethods);

    DLFLError = PyErr_NewException("dlfl.error", NULL, NULL);
    Py_INCREF(DLFLError);
    PyModule_AddObject(dlfl, "error", DLFLError);

		PyObject *dlflDict = PyModule_GetDict(dlfl); 

    /* Initialize the C API pointer array */ 
    PyDLFL_API[0] = (void *)PyDLFL_UsingGUI; 
    PyDLFL_API[1] = (void *)PyDLFL_PassObject;
    /* Create a CObject containing the API pointer array's address */ 
    PyObject *c_api_object = PyCObject_FromVoidPtr((void *)PyDLFL_API, NULL); 
    if (c_api_object != NULL) { 
      /* Create a name for this object in the module’s namespace */ 
      PyDict_SetItemString(dlflDict, "_C_API", c_api_object); 
      Py_DECREF(c_api_object); 
    }
}

