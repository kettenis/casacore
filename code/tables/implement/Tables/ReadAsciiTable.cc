//# ReadAsciiTable.cc: Filling a table from an Ascii file
//# Copyright (C) 1993,1994,1995,1996,1997,1999,2000,2001,2002,2003
//# Associated Universities, Inc. Washington DC, USA.
//# 
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//# 
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//# 
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//# 
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$

#include <aips/Tables/ReadAsciiTable.h>
#include <aips/Tables/TableDesc.h>
#include <aips/Tables/ScaColDesc.h>
#include <aips/Tables/ArrColDesc.h>
#include <aips/Tables/Table.h>
#include <aips/Tables/TableRecord.h>
#include <aips/Tables/TableColumn.h>
#include <aips/Tables/ArrayColumn.h>
#include <aips/Tables/SetupNewTab.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/ArrayUtil.h>
#include <aips/Containers/Block.h>
#include <aips/OS/Path.h>
#include <aips/Utilities/String.h>
#include <aips/Utilities/Regex.h>
#include <aips/Exceptions/Error.h>

#include <aips/Logging/LogIO.h>
#include <aips/Logging/LogOrigin.h>

#include <aips/stdio.h>
#include <aips/string.h>
#include <aips/iostream.h>
#include <aips/fstream.h>             // needed for file IO
#include <aips/sstream.h>           // needed for internal IO


const Int lineSize = 32768;



//# Helper function.
//# Read a line and ignore lines to be skipped.
Bool ReadAsciiTable::getLine (ifstream& file, Int& lineNumber,
			      char* line, Int lineSize,
			      Bool testComment, const Regex& commentMarker,
			      Int firstLine, Int lastLine)
{
  Int dummy;
  while (True) {
    if (! file.getline (line, lineSize)) {
      return False;
    }
    lineNumber++;
    if (lineNumber >= firstLine) {
      if (lastLine <= 0  ||  lineNumber <= lastLine) {
	if (! testComment) {
	  return True;
	}
	if (commentMarker.find (line, lineSize, dummy) != 0) {
	  return True;
	}
      }
    }
  }
}


//# Helper function 
//# It gets the next value from a line and stores it in result.
//# It updates at and returns the length of the value retrieved.
//# Quotes around strings are removed
//# -1 is returned if no more values are found.
Int ReadAsciiTable::getNext (const Char* string, Int strlen, Char* result,
			     Int& at, Char separator)
{
    Int i = 0;
    Bool found  = False;
    Bool quoted = False;
    Char ihave;
    // The next few lines are needed to treat e.g. a trailing comma as
    // a value.
    Bool hasNext = False;
    if (at < 0) {
        at = -at;
	hasNext = True;
    }
    for (; at<strlen; at++) {
	ihave = string[at];
	if (ihave == '"') {
	    if (quoted) {
		quoted = False;
		continue;
	    }else{
		quoted = True;
		found  = True;
		continue;
	    }
	}
	if (ihave == '\0') {
	    result[i] = '\0';
	    return (found||hasNext  ?  i : -1);
	}
	if (quoted) {
	    result[i++] = ihave;
	    continue;
	}
	if (ihave == '\t') {
	    ihave = ' ';
	}
	if (ihave == separator) {
	    if (separator != ' ') {
	        found = True;
	        at++;
		at = -at;      // needed to recognize trailing comma
	    }
	    if (found) {
	        result[i] = '\0';
		return i;
	    }
	}
	if (ihave != ' ') {
	    found = True;
	}
	if (found) {
	    result[i++] = ihave; 
	}
    } 
    return -1;
}



void ReadAsciiTable::getTypes (const IPosition& shape,
			       const Char* in, Int leng,
			       Char* string1, Char* string2, Char separator)
{
    Int at = 0;
    Int i = 0;
    //# When constructing str in the while loop (in the else branch),
    //# a compiler bug appeared on RH systems.
    //# Therefore assignment is used instead.
    String str;
    while (getNext (in, leng, string2, at, separator) >= 0) {
        if (string2[0] == '\0') {
	    string1[0] = 'A';
	} else {
	    str = string2;
	    if (str.matches (RXint)) {
	        string1[0] = 'I';
	    } else if (str.matches (RXdouble)) {
	        string1[0] = 'D';
	    } else {
	        string1[0] = 'A';
	    }
	}
	string1++;
	char name[16];
	i++;
	sprintf (name, " Column%i", i);
	strcpy (string2, name);
	string2 += strlen(name);
	string2[0] = '\0';
	if (shape.nelements() > 0) {
	    ostringstream ostr;
	    for (uInt i=0; i<shape.nelements(); i++) {
	        if (i > 0) {
		    ostr << ',';
		}
	        ostr << shape(i);
	    }
	// There is probably a way to attach the char * to the ostringstream
	// but I'm not going to worry about it. wky 2003/02/27
	    strcpy(string1, ostr.str().data());
	    break;
	}
	string1[0] = ' ';
	string1++;
	string1[0] = '\0';
    }
}



//# Convert a string to a Bool
Bool ReadAsciiTable::makeBool (const String& str)
{
    if (str.length() == 0  ||  str == "0"  ||  str[0] == 'F'
    ||  str[0] == 'f'  || str[0] == 'N'  || str[0] == 'n') {
        return False;
    }
    return True;
}



//# Read a keyword set and add it to keysets.
void ReadAsciiTable::handleKeyset (Int lineSize, char* string1,
				   char* first, char* second,
				   TableRecord& keysets,
				   LogIO& logger,
				   const String& fileName,
				   ifstream& jFile,
				   Int& lineNumber,
				   Char separator,
				   Bool testComment,
				   const Regex& commentMarker,
				   Int firstLine, Int lastLine)
{
  TableRecord keyset;

  // Get the column name in case it is a column keywordset.
  String colName;
  Int atl = 0;
  getNext (string1, lineSize, first, atl, ' '); 
  Int d4 = getNext (string1, lineSize, second, atl, ' '); 
  if (d4 > 0) {
    colName = second;
  }
  while (True) {

// Read the next line(s)

    if (!getLine (jFile, lineNumber, string1, lineSize,
		  testComment, commentMarker,
		  firstLine, lastLine)) {
      throw (AipsError ("No .endkey line in " + fileName));
    }

// If we are at END of KEYWORDS read the next line to get NAMES OF COLUMNS
// or to get next keyword group.

    if (strncmp(string1, ".endkey", 7) == 0) {
      if (!getLine (jFile, lineNumber, string1, lineSize,
		    testComment, commentMarker,
		    firstLine, lastLine)) {
	string1[0] = '\0';
      }
      break;
    }

    // Read the first two fields (name and type) of a KEYWORD line
    Int at3=0;
    Int done3 = getNext (string1, lineSize, first, at3, ' '); 
    Int done4 = getNext (string1, lineSize, second, at3, ' '); 
    if (done3<=0 || done4<=0) {
      throw (AipsError ("No keyword name or type in line " +
			String::toString(lineNumber)
			+ " of " + fileName));
    }
    String keyName = String(first);
    String keyType = String(second);
    keyType.upcase();
    if (keyset.isDefined (keyName)) {
      logger << LogIO::WARN <<
	"Keyword " << keyName << " skipped because defined twice in "
	     << fileName << LogIO::POST;
    } else {
      // Convert the type string to shape and type.
      IPosition keyShape;
      Int keyRAT;
      Int varAxis = getTypeShape (keyType, keyShape, keyRAT);
      // If no shape is given, the keyword can be a vector.
      Bool shpDefined = keyShape.nelements() > 0;
      if (!shpDefined) {
	keyShape = IPosition(1,1);
	varAxis = 0;
      }
      // Get the keyword values from the line and store them in the set.
      switch (keyRAT) {
      case RATBool:
	{
	  Block<Bool> values;
	  IPosition shp = getArray (string1, lineSize, first, at3, separator,
				    keyShape, varAxis, keyRAT, &values);
	  if (!shpDefined  &&  shp(0) == 1) {
	    keyset.define (keyName, values[0]);
	  } else {
	    Array<Bool> array(shp, values.storage(), SHARE);
	    keyset.define (keyName, array);
	  }
	}
	break;
      case RATShort:
	{
	  Block<Short> values;
	  IPosition shp = getArray (string1, lineSize, first, at3, separator,
				    keyShape, varAxis, keyRAT, &values);
	  if (!shpDefined  &&  shp(0) == 1) {
	    keyset.define (keyName, values[0]);
	  } else {
	    Array<Short> array(shp, values.storage(), SHARE);
	    keyset.define (keyName, array);
	  }
	}
	break;
      case RATInt:
	{
	  Block<Int> values;
	  IPosition shp = getArray (string1, lineSize, first, at3, separator,
				    keyShape, varAxis, keyRAT, &values);
	  if (!shpDefined  &&  shp(0) == 1) {
	    keyset.define (keyName, values[0]);
	  } else {
	    Array<Int> array(shp, values.storage(), SHARE);
	    keyset.define (keyName, array);
	  }
	}
	break;
      case RATFloat:
	{
	  Block<Float> values;
	  IPosition shp = getArray (string1, lineSize, first, at3, separator,
				    keyShape, varAxis, keyRAT, &values);
	  if (!shpDefined  &&  shp(0) == 1) {
	    keyset.define (keyName, values[0]);
	  } else {
	    Array<Float> array(shp, values.storage(), SHARE);
	    keyset.define (keyName, array);
	  }
	}
	break;
      case RATDouble:
	{
	  Block<Double> values;
	  IPosition shp = getArray (string1, lineSize, first, at3, separator,
				    keyShape, varAxis, keyRAT, &values);
	  if (!shpDefined  &&  shp(0) == 1) {
	    keyset.define (keyName, values[0]);
	  } else {
	    Array<Double> array(shp, values.storage(), SHARE);
	    keyset.define (keyName, array);
	  }
	}
	break;
      case RATString:
	{
	  Block<String> values;
	  IPosition shp = getArray (string1, lineSize, first, at3, separator,
				    keyShape, varAxis, keyRAT, &values);
	  if (!shpDefined  &&  shp(0) == 1) {
	    keyset.define (keyName, values[0]);
	  } else {
	    Array<String> array(shp, values.storage(), SHARE);
	    keyset.define (keyName, array);
	  }
	}
	break;
      case RATComX:
      case RATComZ:
	{
	  Block<Complex> values;
	  IPosition shp = getArray (string1, lineSize, first, at3, separator,
				    keyShape, varAxis, keyRAT, &values);
	  if (!shpDefined  &&  shp(0) == 1) {
	    keyset.define (keyName, values[0]);
	  } else {
	    Array<Complex> array(shp, values.storage(), SHARE);
	    keyset.define (keyName, array);
	  }
	}
	break;
      case RATDComX:
      case RATDComZ:
	{
	  Block<DComplex> values;
	  IPosition shp = getArray (string1, lineSize, first, at3, separator,
				    keyShape, varAxis, keyRAT, &values);
	  if (!shpDefined  &&  shp(0) == 1) {
	    keyset.define (keyName, values[0]);
	  } else {
	    Array<DComplex> array(shp, values.storage(), SHARE);
	    keyset.define (keyName, array);
	  }
	}
	break;
      }
    }
  }
  if (keysets.isDefined (colName)) {
    logger << LogIO::WARN
	   << "Keywordset of column " << colName
	   << " skipped because defined twice in "
	   << fileName << LogIO::POST;
  } else {
    keysets.defineRecord (colName, keyset);
  }
}


Int ReadAsciiTable::getTypeShape (const String& typestr,
				  IPosition& shape, Int& type)
{
  shape.resize (0);
  Int varAxis = -1;
  // Split at each comma.
  Vector<String> vec = stringToVector (typestr);
  // The first value can be something like I10, so find first digit.
  // It should have a type before the first digit.
  uInt pos = vec(0).find (Regex("[0-9]"));
  if (pos == 0) {
    throw AipsError ("No type info in type string '" + typestr + "'");
  }
  // Get type without shape info.
  // Note: need to convert pos to an Int because some compilers are more picky about
  // type safety, i.e. the native compilers for SGI and SUN
  String tp = vec(0).before (Int(pos));
  if (pos >= vec(0).length()) {
    vec(0) = String();
    // Clear vector if no shape given at all.
    if (vec.nelements() == 1) {
      vec.resize (0);
    }
  } else {
    // Keep only length in first value.
    vec(0) = vec(0).from(Int(pos));
  }
  shape.resize (vec.nelements());
  Regex num("[0-9]+");
  // Check value and convert to integers.
  // One variable shaped axis is possible.
  for (uInt i=0; i<vec.nelements(); i++) {
    if (! vec(i).matches (num)) {
      throw AipsError ("Invalid shape value '" + vec(i) +
		       "' in type string '" + typestr + "'");
    }
    istringstream istr(vec(i));
    istr >> shape(i);
    if (shape(i) <= 0) {
      if (varAxis >= 0) {
	throw AipsError ("Multiple variable axes in type string '"
			 + typestr + "'");
      }
      varAxis = i;
      shape(i) = 1;
    }
  }
  if (tp == "B") {
    type = RATBool;
  } else if (tp == "S") {
    type = RATShort;
  } else if (tp == "I") {
    type = RATInt;
  } else if (tp == "R") {
    type = RATFloat;
  } else if (tp == "D") {
    type = RATDouble;
  } else if (tp == "A") {
    type = RATString;
  } else if (tp == "X") {
    type = RATComX;
  } else if (tp == "Z") {
    type = RATComZ;
  } else if (tp == "DX") {
    type = RATDComX;
  } else if (tp == "DZ") {
    type = RATDComZ;
  } else {
    throw AipsError ("Invalid type specifier '" + tp + "'");
  }
  return varAxis;
}


Bool ReadAsciiTable::getValue (char* string1, Int lineSize, char* first,
			       Int& at1, Char separator,
			       Int type, void* value)
{
  Float f1=0, f2=0;
  Double d1=0, d2=0;
  Bool more = True;
  Int done1 = getNext (string1, lineSize, first, at1, separator);
  if (done1 < 0) {
    more = False;
    done1 = 0;
    first[0] = '\0';
  }
  if(more){
  String dum(first, done1);
  switch (type) {
  case RATBool:
    *(Bool*)value = makeBool(String(first, done1));
    break;
  case RATShort:
    if (done1 > 0) {
      istringstream(dum) >> *(Short*)value;
    } else {
      *(Short*)value = 0;
    }
    break;
  case RATInt:
    if (done1 > 0) {
      istringstream(dum) >> *(Int*)value;
    } else {
      *(Int*)value = 0;
    }
    break;
  case RATFloat:
    if (done1 > 0) {
      istringstream(dum) >> *(Float*)value;
    } else {
      *(Float*)value = 0;
    }
    break;
  case RATDouble:
    if (done1 > 0) {
      istringstream(dum) >> *(Double*)value;
    } else {
      *(Double*)value = 0;
    }
    break;
  case RATString:
    *(String*)value = String(first, done1);
    break;
  case RATComX:
    if (done1 > 0) {
      istringstream(dum) >> f1;
    }
    done1 = getNext (string1, lineSize, first, at1, separator);
    if (done1 > 0) {
      String dum2(first, done1);
      istringstream(dum2) >> f2;
    }
    *(Complex*)value = Complex(f1, f2);
    break;
  case RATDComX:
    if (done1 > 0) {
      istringstream(dum) >> d1;
    }
    done1 = getNext (string1, lineSize, first, at1, separator);
    if (done1 > 0) {
      String dum2(first, done1);
      istringstream(dum2) >> d2;
    }
    *(DComplex*)value = DComplex(d1, d2);
    break;
  case RATComZ:
    if (done1 > 0) {
      istringstream(dum) >> f1;
    }
    done1 = getNext (string1, lineSize, first, at1, separator);
    if (done1 > 0) {
      String dum2(first, done1);
      istringstream(dum2) >> f2;
    }
    f2 *= 3.14159265/180.0; 
    *(Complex*)value = Complex(f1*cos(f2), f1*sin(f2));
    break;
  case RATDComZ:
    if (done1 > 0) {
      istringstream(dum) >> d1;
    }
    done1 = getNext (string1, lineSize, first, at1, separator);
    if (done1 > 0) {
      String dum2(first, done1);
      istringstream(dum2) >> d2;
    }
    d2 *= 3.14159265/180.0; 
    *(DComplex*)value = DComplex(d1*cos(d2), d1*sin(d2));
    break;
  }
  }
  return more;
}


void ReadAsciiTable::handleScalar (char* string1, Int lineSize, char* first,
				   Int& at1, Char separator,
				   Int type,
				   TableColumn& tabcol, uInt rownr)
{
  switch (type) {
  case RATBool:
    {
      Bool value;
      getValue (string1, lineSize, first, at1, separator, type, &value);
      tabcol.putScalar (rownr, value);
    }
    break;
  case RATShort:
    {
      Short value;
      getValue (string1, lineSize, first, at1, separator, type, &value);
      tabcol.putScalar (rownr, value);
    }
    break;
  case RATInt:
    {
      Int value;
      getValue (string1, lineSize, first, at1, separator, type, &value);
      tabcol.putScalar (rownr, value);
    }
    break;
  case RATFloat:
    {
      Float value;
      getValue (string1, lineSize, first, at1, separator, type, &value);
      tabcol.putScalar (rownr, value);
    }
    break;
  case RATDouble:
    {
      Double value;
      getValue (string1, lineSize, first, at1, separator, type, &value);
      tabcol.putScalar (rownr, value);
    }
    break;
  case RATString:
    {
      String value;
      getValue (string1, lineSize, first, at1, separator, type, &value);
      tabcol.putScalar (rownr, value);
    }
    break;
  case RATComX:
  case RATComZ:
    {
      Complex value;
      getValue (string1, lineSize, first, at1, separator, type, &value);
      tabcol.putScalar (rownr, value);
    }
    break;
  case RATDComX:
  case RATDComZ:
    {
      DComplex value;
      getValue (string1, lineSize, first, at1, separator, type, &value);
      tabcol.putScalar (rownr, value);
    }
    break;
  }
}


IPosition ReadAsciiTable::getArray (char* string1, Int lineSize, char* first,
				    Int& at1, Char separator,
				    const IPosition& shape, Int varAxis,
				    Int type, void* valueBlock)
{
  IPosition shp(shape);
  uInt nelem = shp.product();
  uInt nfound = 0;
  switch (type) {
  case RATBool:
    {
      Block<Bool>& data = *(Block<Bool>*)valueBlock;
      data.resize (nelem);
      data = False;
      Bool value;
      while (getValue (string1, lineSize, first, at1, separator,
		       type, &value)) {
	if (nfound == data.nelements()) {
	  data.resize (2*nfound, True, True);
	}
	data[nfound++] = value;
	if (varAxis < 0  &&  nfound == data.nelements()) {
	  break;
	}
      }
      if (varAxis >= 0) {
	shp(varAxis) = (nfound + nelem - 1) / nelem;
	nelem = shp.product();
	if (nelem > nfound) {
	  if (nelem > data.nelements()) {
	    data.resize (nelem, True, True);
	  }
	  objset (&data[nfound], False, nelem-nfound);
	}
      }
    }
    break;
  case RATShort:
    {
      Block<Short>& data = *(Block<Short>*)valueBlock;
      data.resize (nelem);
      data = Short(0);
      Short value;
      while (getValue (string1, lineSize, first, at1, separator,
		       type, &value)) {
	if (nfound == data.nelements()) {
	  data.resize (2*nfound, True, True);
	}
	data[nfound++] = value;
	if (varAxis < 0  &&  nfound == data.nelements()) {
	  break;
	}
      }
      if (varAxis >= 0) {
	shp(varAxis) = (nfound + nelem - 1) / nelem;
	nelem = shp.product();
	if (nelem > nfound) {
	  if (nelem > data.nelements()) {
	    data.resize (nelem, True, True);
	  }
	  objset (&data[nfound], Short(0), nelem-nfound);
	}
      }
    }
    break;
  case RATInt:
    {
      Block<Int>& data = *(Block<Int>*)valueBlock;
      data.resize (nelem);
      data = False;
      Int value;
      while (getValue (string1, lineSize, first, at1, separator,
		       type, &value)) {
	if (nfound == data.nelements()) {
	  data.resize (2*nfound, True, True);
	}
	data[nfound++] = value;
	if (varAxis < 0  &&  nfound == data.nelements()) {
	  break;
	}
      }
      if (varAxis >= 0) {
	shp(varAxis) = (nfound + nelem - 1) / nelem;
	nelem = shp.product();
	if (nelem > nfound) {
	  if (nelem > data.nelements()) {
	    data.resize (nelem, True, True);
	  }
	  objset (&data[nfound], 0, nelem-nfound);
	}
      }
    }
    break;
  case RATFloat:
    {
      Block<Float>& data = *(Block<Float>*)valueBlock;
      data.resize (nelem);
      data = Float(0);
      Float value;
      while (getValue (string1, lineSize, first, at1, separator,
		       type, &value)) {
	if (nfound == data.nelements()) {
	  data.resize (2*nfound, True, True);
	}
	data[nfound++] = value;
	if (varAxis < 0  &&  nfound == data.nelements()) {
	  break;
	}
      }
      if (varAxis >= 0) {
	shp(varAxis) = (nfound + nelem - 1) / nelem;
	nelem = shp.product();
	if (nelem > nfound) {
	  if (nelem > data.nelements()) {
	    data.resize (nelem, True, True);
	  }
	  objset (&data[nfound], Float(0), nelem-nfound);
	}
      }
    }
    break;
  case RATDouble:
    {
      Block<Double>& data = *(Block<Double>*)valueBlock;
      data.resize (nelem);
      data = Double(0);
      Double value;
      while (getValue (string1, lineSize, first, at1, separator,
		       type, &value)) {
	if (nfound == data.nelements()) {
	  data.resize (2*nfound, True, True);
	}
	data[nfound++] = value;
	if (varAxis < 0  &&  nfound == data.nelements()) {
	  break;
	}
      }
      if (varAxis >= 0) {
	shp(varAxis) = (nfound + nelem - 1) / nelem;
	nelem = shp.product();
	if (nelem > nfound) {
	  if (nelem > data.nelements()) {
	    data.resize (nelem, True, True);
	  }
	  objset (&data[nfound], Double(0), nelem-nfound);
	}
      }
    }
    break;
  case RATString:
    {
      Block<String>& data = *(Block<String>*)valueBlock;
      data.resize (nelem);
      data = String();
      String value;
      while (getValue (string1, lineSize, first, at1, separator,
		       type, &value)) {
	if (nfound == data.nelements()) {
	  data.resize (2*nfound, True, True);
	}
	data[nfound++] = value;
	if (varAxis < 0  &&  nfound == data.nelements()) {
	  break;
	}
      }
      if (varAxis >= 0) {
	shp(varAxis) = (nfound + nelem - 1) / nelem;
	nelem = shp.product();
	if (nelem > nfound) {
	  if (nelem > data.nelements()) {
	    data.resize (nelem, True, True);
	  }
	  objset (&data[nfound], String(), nelem-nfound);
	}
      }
    }
    break;
  case RATComX:
  case RATComZ:
    {
      Block<Complex>& data = *(Block<Complex>*)valueBlock;
      data.resize (nelem);
      data = Complex();
      Complex value;
      while (getValue (string1, lineSize, first, at1, separator,
		       type, &value)) {
	if (nfound == data.nelements()) {
	  data.resize (2*nfound, True, True);
	}
	data[nfound++] = value;
	if (varAxis < 0  &&  nfound == data.nelements()) {
	  break;
	}
      }
      if (varAxis >= 0) {
	shp(varAxis) = (nfound + nelem - 1) / nelem;
	nelem = shp.product();
	if (nelem > nfound) {
	  if (nelem > data.nelements()) {
	    data.resize (nelem, True, True);
	  }
	  objset (&data[nfound], Complex(), nelem-nfound);
	}
      }
    }
    break;
  case RATDComX:
  case RATDComZ:
    {
      Block<DComplex>& data = *(Block<DComplex>*)valueBlock;
      data.resize (nelem);
      data = DComplex();
      DComplex value;
      while (getValue (string1, lineSize, first, at1, separator,
		       type, &value)) {
	if (nfound == data.nelements()) {
	  data.resize (2*nfound, True, True);
	}
	data[nfound++] = value;
	if (varAxis < 0  &&  nfound == data.nelements()) {
	  break;
	}
      }
      if (varAxis >= 0) {
	shp(varAxis) = (nfound + nelem - 1) / nelem;
	nelem = shp.product();
	if (nelem > nfound) {
	  if (nelem > data.nelements()) {
	    data.resize (nelem, True, True);
	  }
	  objset (&data[nfound], DComplex(), nelem-nfound);
	}
      }
    }
    break;
  }
  return shp;
}


void ReadAsciiTable::handleArray (char* string1, Int lineSize, char* first,
				  Int& at1, Char separator,
				  const IPosition& shape, Int varAxis,
				  Int type,
				  TableColumn& tabcol, uInt rownr)
{
  switch (type) {
  case RATBool:
    {
      Block<Bool> data;
      IPosition shp = getArray (string1, lineSize, first, at1, separator,
				shape, varAxis, type, &data);
      Array<Bool> array(shp, data.storage(), SHARE);
      ArrayColumn<Bool>(tabcol).put (rownr, array);
    }
    break;
  case RATShort:
    {
      Block<Short> data;
      IPosition shp = getArray (string1, lineSize, first, at1, separator,
				shape, varAxis, type, &data);
      Array<Short> array(shp, data.storage(), SHARE);
      ArrayColumn<Short>(tabcol).put (rownr, array);
    }
    break;
  case RATInt:
    {
      Block<Int> data;
      IPosition shp = getArray (string1, lineSize, first, at1, separator,
				shape, varAxis, type, &data);
      Array<Int> array(shp, data.storage(), SHARE);
      ArrayColumn<Int>(tabcol).put (rownr, array);
    }
    break;
  case RATFloat:
    {
      Block<Float> data;
      IPosition shp = getArray (string1, lineSize, first, at1, separator,
				shape, varAxis, type, &data);
      Array<Float> array(shp, data.storage(), SHARE);
      ArrayColumn<Float>(tabcol).put (rownr, array);
    }
    break;
  case RATDouble:
    {
      Block<Double> data;
      IPosition shp = getArray (string1, lineSize, first, at1, separator,
				shape, varAxis, type, &data);
      Array<Double> array(shp, data.storage(), SHARE);
      ArrayColumn<Double>(tabcol).put (rownr, array);
    }
    break;
  case RATString:
    {
      Block<String> data;
      IPosition shp = getArray (string1, lineSize, first, at1, separator,
				shape, varAxis, type, &data);
      Array<String> array(shp, data.storage(), SHARE);
      ArrayColumn<String>(tabcol).put (rownr, array);
    }
    break;
  case RATComX:
  case RATComZ:
    {
      Block<Complex> data;
      IPosition shp = getArray (string1, lineSize, first, at1, separator,
				shape, varAxis, type, &data);
      Array<Complex> array(shp, data.storage(), SHARE);
      ArrayColumn<Complex>(tabcol).put (rownr, array);
    }
    break;
  case RATDComX:
  case RATDComZ:
    {
      Block<DComplex> data;
      IPosition shp = getArray (string1, lineSize, first, at1, separator,
				shape, varAxis, type, &data);
      Array<DComplex> array(shp, data.storage(), SHARE);
      ArrayColumn<DComplex>(tabcol).put (rownr, array);
    }
    break;
  }
}


String ReadAsciiTable::doRun (const String& headerfile, const String& filein, 
			      const String& tableproto,
			      const String& tablename,
			      Bool autoHeader, const IPosition& autoShape,
			      Char separator,
			      Bool testComment, const Regex& commentMarker,
			      Int firstLine, Int lastLine)
{
    char  string1[lineSize], string2[lineSize], stringsav[lineSize];
    char  first[lineSize], second[lineSize];
    Block<String>  nameOfColumn(100);
    Block<String>  tstrOfColumn(100);
    String  keyName;

    LogIO logger(LogOrigin("readAsciiTable", WHERE));

// Determine if header and data are in one file.
    Bool oneFile = (headerfile == filein);
    Int firstHeaderLine = 1;
    Int lastHeaderLine = -1;
    if (oneFile) {
        firstHeaderLine = firstLine;
	lastHeaderLine  = lastLine;
    }

// PART ONE
// Define the TABLE description, i.e. define its columns.
// Create the description as scratch if no name is given.

    TableDesc td (tableproto,
		  (tableproto.empty() ? TableDesc::Scratch : TableDesc::New));

    ifstream jFile;
    Path headerPath(headerfile);
    String hdrName = headerPath.expandedName();
    jFile.open(hdrName.chars(), ios::in);
    if (! jFile) {
        throw (AipsError ("Cannot open header file " + hdrName));
    }

// Read the first line. It will be KEYWORDS or NAMES OF COLUMNS

    Int lineNumber = 0;
    if (!getLine (jFile, lineNumber, string1, lineSize,
		  testComment, commentMarker,
		  firstHeaderLine, lastHeaderLine)) {
	throw (AipsError ("Cannot read first header line of " + headerfile));
    }

// If the first line shows that we have KEYWORDS read until the
// end of keywords while assembling the keywords.

    TableRecord keysets;
    while (strncmp(string1, ".key", 4) == 0) {
        handleKeyset (lineSize, string1, first, second,
		      keysets, logger,
		      headerfile, jFile, lineNumber, separator,
		      testComment, commentMarker,
		      firstHeaderLine, lastHeaderLine);
    }

// Okay, all keywords have been read.
// string1 contains the next line (if any).
// Read the column definition lines from header file (if needed).
// Determine the types if autoheader is given.

// Previous line should be NAMES OF COLUMNS; now get TYPE OF COLUMNS line
    if (!autoHeader) {
        if (string1[0] == '\0') {
	    throw (AipsError("No COLUMN NAMES line in " + headerfile));
	}
	if (!getLine (jFile, lineNumber, string2, lineSize,
		      testComment, commentMarker,
		      firstHeaderLine, lastHeaderLine)) {
	    throw (AipsError("No COLUMN TYPES line in " + headerfile));
	}
    }

// Now open the actual data file (if not the same as header file).
// Read the first line if auto header.

    if (!oneFile) {
        jFile.close();
	Path filePath(filein);
	String fileName = filePath.expandedName();
	jFile.open(fileName.chars(), ios::in);
	if (! jFile) {
	    throw (AipsError ("Cannot open input file " + fileName));
	}
	lineNumber = 0;
	if (autoHeader) {
	    if (!getLine (jFile, lineNumber, string1, lineSize,
			  testComment, commentMarker,
			  firstLine, lastLine)) {
	        string1[0] = '\0';
	    }
	}
    }

    // Process the auto header.
    // Save string, because it'll be overwritten.
    stringsav[0] = '\0';
    if (autoHeader) {
        strcpy (stringsav, string1);
        getTypes (autoShape, string1, lineSize, string2, first, separator);
	strcpy (string1, first);
    }

// Break up the NAME OF COLUMNS line and the TYPE OF COLUMNS line
// Place the results in the two arrays.
// Also put in in a single string to be returned to the caller.

    String formStr;
    Int done1 = 0, done2 = 0, at1 = 0, at2 = 0, nrcol = 0;
    while (done1 >= 0) {
	done1 = getNext (string1, lineSize, first, at1, ' ');
	done2 = getNext (string2, lineSize, second, at2, ' ');
	if (done1>0 && done2>0) {
	    if (nrcol >= Int(nameOfColumn.nelements())) {
	        nameOfColumn.resize (2*nrcol, True, True);
	        tstrOfColumn.resize (2*nrcol, True, True);
	    }
	    nameOfColumn[nrcol] = String(first);
	    tstrOfColumn[nrcol] = String(second);
	    tstrOfColumn[nrcol].upcase();
	    if (! formStr.empty()) {
	        formStr += ", ";
	    }
	    formStr += nameOfColumn[nrcol] + "=" + tstrOfColumn[nrcol];
	    nrcol++;
	} else if (done1>=0 || done2>=0) {
	    throw (AipsError ("Mismatching COLUMN NAMES and TYPES lines in "
			      + headerfile));
	}
    }

// Create the TABLE Columns for these variables

    Block<IPosition> shapeOfColumn(nrcol);
    Block<Int>       typeOfColumn(nrcol);
    Int              varAxis;

    for (Int i5=0; i5<nrcol; i5++) {
        varAxis = getTypeShape (tstrOfColumn[i5],
				shapeOfColumn[i5],
				typeOfColumn[i5]);
	if (varAxis >= 0  &&  i5 != nrcol-1) {
	  throw AipsError ("Only last column can have variable shaped arrays");
	}
	if (shapeOfColumn[i5].nelements() > 0) {
	  IPosition shape;
	  Int option = 0;
	  if (varAxis < 0) {
	    shape = shapeOfColumn[i5];
	    option = ColumnDesc::Direct | ColumnDesc::FixedShape;
	  }
	  switch (typeOfColumn[i5]) {
	  case RATBool:
	    td.addColumn (ArrayColumnDesc<Bool> (nameOfColumn[i5],
						 shape, option));
	    break;
	  case RATShort:
	    td.addColumn (ArrayColumnDesc<Short> (nameOfColumn[i5],
						  shape, option));
	    break;
	  case RATInt:
	    td.addColumn (ArrayColumnDesc<Int> (nameOfColumn[i5],
						shape, option));
	    break;
	  case RATFloat:
	    td.addColumn (ArrayColumnDesc<Float> (nameOfColumn[i5],
						  shape, option));
	    break;
	  case RATDouble:
	    td.addColumn (ArrayColumnDesc<Double> (nameOfColumn[i5],
						   shape, option));
	    break;
	  case RATString:
	    td.addColumn (ArrayColumnDesc<String> (nameOfColumn[i5],
						   shape, option));
	    break;
	  case RATComX:
	  case RATComZ:
	    td.addColumn (ArrayColumnDesc<Complex> (nameOfColumn[i5],
						     shape, option));
	    break;
	  case RATDComX:
	  case RATDComZ:
	    td.addColumn (ArrayColumnDesc<DComplex> (nameOfColumn[i5],
						     shape, option));
	    break;
	  }
	} else {
	  switch (typeOfColumn[i5]) {
	  case RATBool:
	    td.addColumn (ScalarColumnDesc<Bool> (nameOfColumn[i5]));
	    break;
	  case RATShort:
	    td.addColumn (ScalarColumnDesc<Short> (nameOfColumn[i5]));
	    break;
	  case RATInt:
	    td.addColumn (ScalarColumnDesc<Int> (nameOfColumn[i5]));
	    break;
	  case RATFloat:
	    td.addColumn (ScalarColumnDesc<Float> (nameOfColumn[i5]));
	    break;
	  case RATDouble:
	    td.addColumn (ScalarColumnDesc<Double> (nameOfColumn[i5]));
	    break;
	  case RATString:
	    td.addColumn (ScalarColumnDesc<String> (nameOfColumn[i5]));
	    break;
	  case RATComX:
	  case RATComZ:
	    td.addColumn (ScalarColumnDesc<Complex> (nameOfColumn[i5]));
	    break;
	  case RATDComX:
	  case RATDComZ:
	    td.addColumn (ScalarColumnDesc<DComplex> (nameOfColumn[i5]));
	    break;
	  }
	}
    }



// PART TWO
// The TableDesc has now been created.  Start filling in the Table.
// Use the default storage manager.

    SetupNewTable newtab(tablename, td, Table::New);
    Table tab(newtab);

// Write keywordsets.

    for (uInt i=0; i<keysets.nfields(); i++) {
        String colnm = keysets.name(i);
        if (colnm.empty()) {
	    tab.rwKeywordSet() = keysets.subRecord (i);
	} else {
	    if (!tab.tableDesc().isColumn (colnm)) {
	        logger << LogIO::WARN
		       << "Keywordset of column " << colnm
		       << " skipped because column is not defined in "
		       << headerfile << LogIO::POST;
	    } else {
	        TableColumn tabcol (tab, colnm);
		tabcol.rwKeywordSet() = keysets.subRecord (i);
	    }
	}
    }

    TableColumn* tabcol = new TableColumn[nrcol];
    if (tabcol == 0) {
	throw (AllocError ("readAsciiTable", nrcol));
    }
    for (Int i=0; i<nrcol; i++) {
	tabcol[i].reference (TableColumn (tab, nameOfColumn[i]));
    }
    uInt rownr = 0;

// OK, Now we have real data
// stringsav may contain the first data line.

    Bool cont = True;
    if (stringsav[0] == '\0') {
        cont = getLine (jFile, lineNumber, string1, lineSize,
			testComment, commentMarker,
			firstLine, lastLine);
    } else {
        strcpy (string1, stringsav);
    }
    while (cont) {
	at1 = 0; 
	tab.addRow();
	for (Int i6=0; i6<nrcol; i6++) {
	    if (shapeOfColumn[i6].nelements() > 0) {
	        Int varAx = (i6 == nrcol-1  ?  varAxis : -1);
		handleArray (string1, lineSize, first,
			     at1, separator,
			     shapeOfColumn[i6], varAx,
			     typeOfColumn[i6],
			     tabcol[i6], rownr);
	    } else {
		handleScalar (string1, lineSize, first,
			      at1, separator,
			      typeOfColumn[i6],
			      tabcol[i6], rownr);
	    }
	}
	rownr++;
        cont = getLine (jFile, lineNumber, string1, lineSize,
			testComment, commentMarker,
			firstLine, lastLine);
    }

    delete [] tabcol;
    jFile.close();
    return formStr;
}


String ReadAsciiTable::run (const String& headerfile, const String& filein, 
			    const String& tableproto, const String& tablename,
			    Bool autoHeader, const IPosition& autoShape,
			    Char separator,
			    const String& commentMarkerRegex,
			    Int firstLine, Int lastLine)
{
  if (firstLine < 1) {
    firstLine = 1;
  }
  if (commentMarkerRegex.empty()) {
    return doRun (headerfile, filein, tableproto, tablename,
		  autoHeader, autoShape, separator,
		  False, Regex(),
		  firstLine, lastLine);
  } else {
    return doRun (headerfile, filein, tableproto, tablename,
		  autoHeader, autoShape, separator,
		  True, Regex(commentMarkerRegex),
		  firstLine, lastLine);
  }
}

String readAsciiTable (const String& filein, const String& tableproto,
		       const String& tablename, Bool autoHeader,
		       Char separator, const String& commentMarkerRegex,
		       Int firstLine, Int lastLine,
		       const IPosition& autoShape)
{
  return ReadAsciiTable::run (filein, filein, tableproto, tablename,
			      autoHeader, autoShape,
			      separator, commentMarkerRegex,
			      firstLine, lastLine);
}

String readAsciiTable (const String& headerfile, const String& filein,
		       const String& tableproto, const String& tablename,
		       Char separator, const String& commentMarkerRegex,
		       Int firstLine, Int lastLine)
{
  return ReadAsciiTable::run (headerfile, filein, tableproto, tablename,
			      False, IPosition(),
			      separator, commentMarkerRegex,
			      firstLine, lastLine);
}

String readAsciiTable (const String& headerfile, const String& filein,
		       const String& tableproto, const char* tablename,
		       Char separator, const String& commentMarkerRegex,
		       Int firstLine, Int lastLine)
{
  return ReadAsciiTable::run (headerfile, filein, tableproto,
			      String(tablename),
			      False, IPosition(),
			      separator, commentMarkerRegex,
			      firstLine, lastLine);
}
