/***************************************************************************
    File                 : XYEquationCurve.cpp
    Project              : LabPlot
    Description          : A xy-curve defined by a mathematical equation
    --------------------------------------------------------------------
    Copyright            : (C) 2014 Alexander Semke (alexander.semke*web.de)
                           
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

/*!
  \class XYEquationCurve
  \brief A xy-curve defined by a mathematical equation

  \ingroup worksheet
*/

#include "XYEquationCurve.h"
#include "XYEquationCurvePrivate.h"
#include "backend/core/AbstractColumn.h"
#include "backend/core/column/Column.h"
#include "backend/lib/commandtemplates.h"

#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>

#include "backend/gsl/parser_struct.h"
#include "backend/gsl/parser_extern.h"


#include <KIcon>

XYEquationCurve::XYEquationCurve(const QString& name)
		: XYCurve(name, new XYEquationCurvePrivate(this)){
// 	init();
}

XYEquationCurve::XYEquationCurve(const QString& name, XYEquationCurvePrivate* dd)
		: XYCurve(name, dd){
// 	init();
}

 
XYEquationCurve::~XYEquationCurve() {
	//no need to delete the d-pointer here - it inherits from QGraphicsItem
	//and is deleted during the cleanup in QGraphicsScene
}

/*!
	Returns an icon to be used in the project explorer.
*/
QIcon XYEquationCurve::icon() const {
	return KIcon("xy-equation-curve");
}

//##############################################################################
//##########################  getter methods  ##################################
//##############################################################################
BASIC_SHARED_D_READER_IMPL(XYEquationCurve, XYEquationCurve::EquationData, equationData, equationData)

//##############################################################################
//#################  setter methods and undo commands ##########################
//##############################################################################
STD_SETTER_CMD_IMPL_F_S(XYEquationCurve, SetEquationData, XYEquationCurve::EquationData, equationData, recalculate);
void XYEquationCurve::setEquationData(const XYEquationCurve::EquationData& equationData) {
	Q_D(XYEquationCurve);
	if ( (equationData.expression1 != d->equationData.expression1)
		|| (equationData.expression2 != d->equationData.expression2)
		|| (equationData.min != d->equationData.min)
		|| (equationData.max != d->equationData.max)
		|| (equationData.count != d->equationData.count) )
		exec(new XYEquationCurveSetEquationDataCmd(d, equationData, i18n("%1: set equation")));
}

//##############################################################################
//######################### Private implementation #############################
//##############################################################################
XYEquationCurvePrivate::XYEquationCurvePrivate(XYEquationCurve* owner) : XYCurvePrivate(owner),
	xColumn(new Column("x", AbstractColumn::Numeric)),
	yColumn(new Column("y", AbstractColumn::Numeric)),
	q(owner)  {
}

XYEquationCurvePrivate::~XYEquationCurvePrivate() {
	delete xColumn;
	delete yColumn;
}


void XYEquationCurvePrivate::recalculate() {
// 	QString fun;
// 	fun.remove(QRegExp(".*="));		// remove any "xyz =" before expression

	//resize columns
	//TODO
// 	xColumn->setRowCount(equationData.count);
// 	xColumn->setRowCount(equationData.count);

	if (equationData.count<1)
		return;

	//TODO: solve linking problem!
/*
	init_table();

	if (equationData.type == XYEquationCurve::Cartesian) {
		double xMin = parse( equationData.max.toAscii().data() );
		double xMax = parse( equationData.min.toAscii().data() );
		double step = (xMax-xMin)/(double)(equationData.count-1);
		char*  func = equationData.expression1.toAscii().data();
		double x, y;
		char xVar[] = "x";

		for(int i = 0;i < equationData.count; i++) {
			x = xMin + step*i;
			assign_variable(xVar,x);
			gsl_set_error_handler_off();
			y = parse(func);

			if(parse_errors()>0) {
				delete_table();
				return;//TODO
			}

			xColumn->setValueAt(i, x);
			if (finite(y))
				yColumn->setValueAt(i, y);
			else
				yColumn->setValueAt(i, NAN);
		}
	}

	delete_table();
	*/
}

//##############################################################################
//##################  Serialization/Deserialization  ###########################
//##############################################################################
//! Save as XML
void XYEquationCurve::save(QXmlStreamWriter* writer) const{
// 	Q_D(const XYEquationCurve);
// 
//     writer->writeStartElement( "xyEquationCurve" );
// 	XYEquationCurve::save(writer);
// 	writer->writeEndElement(); //close "xyCurve" section
}

//! Load from XML
bool XYEquationCurve::load(XmlStreamReader* reader){
// 	Q_D(XYEquationCurve);
// 
//     if(!reader->isStartElement() || reader->name() != "xyEquationCurve"){
//         reader->raiseError(i18n("no xy equation curve element found"));
//         return false;
//     }
// 
//     if (!readBasicAttributes(reader))
//         return false;
// 
//     QString attributeWarning = i18n("Attribute '%1' missing or empty, default value is used");
//     QXmlStreamAttributes attribs;
//     QString str;
// 
//     while (!reader->atEnd()){
//         reader->readNext();
//         if (reader->isEndElement() && reader->name() == "xyEquationCurve")
//             break;
// 
//         if (!reader->isStartElement())
//             continue;
// 
// 	}

	return true;
}