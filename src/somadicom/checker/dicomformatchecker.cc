/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL-B license under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL-B license as circulated by CEA, CNRS
 * and INRIA at the following URL "http://www.cecill.info".
 *
 * As a counterpart to the access to the source code and  rights to copy,
 * modify and redistribute granted by the license, users are provided only
 * with a limited warranty  and the software's author,  the holder of the
 * economic rights,  and the successive licensors  have only  limited
 * liability.
 *
 * In this respect, the user's attention is drawn to the risks associated
 * with loading,  using,  modifying and/or developing or reproducing the
 * software by the user in light of its specific status of free software,
 * that may mean  that it is complicated to manipulate,  and  that  also
 * therefore means  that it is reserved for developers  and  experienced
 * professionals having in-depth computer knowledge. Users are therefore
 * encouraged to load and test the software's suitability as regards their
 * requirements in conditions enabling the security of their systems and/or
 * data to be ensured and,  more generally, to use and operate it in the
 * same conditions as regards security.
 *
 * The fact that you are presently reading this means that you have had
 * knowledge of the CeCILL-B license and that you accept its terms.
 */

//--- plugin -----------------------------------------------------------------
#include <soma-io/checker/dicomformatchecker.h>             // class declaration
#include <soma-io/Dicom/DicomIO.h>
#include <soma-io/Container/DataInfo.h>
//--- soma-io ----------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/datasourceinfo/datasourceinfoloader.h>
#include <soma-io/datasourceinfo/datasourceinfo.h>
#include <soma-io/datasource/datasourcelist.h>
#include <soma-io/datasource/filedatasource.h>  // because we use file sources
#include <soma-io/reader/itemreader.h>                  // to read in the file
#include <soma-io/utilities/asciidatasourcetraits.h>     // to read datasource
#include <soma-io/writer/pythonwriter.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/object/object.h>                                 // header
#include <cartobase/object/property.h>                               // header
#include <cartobase/stream/fileutil.h>             // to manipulate file names
//--- system -----------------------------------------------------------------
#include <stdio.h>
#define SOMAIO_BYTE_ORDER 0x41424344 //"ABCD" in ascii -> used for byteswap
//--- debug ------------------------------------------------------------------
#include <cartobase/config/verbose.h>
#define localMsg( message ) cartoCondMsg( 4, message, "DICOMFORMATCHECKER" )
// localMsg must be undef at end of file
//----------------------------------------------------------------------------

using namespace soma;
using namespace carto;
using namespace std;


//============================================================================
//   P R I V A T E   M E T H O D S
//============================================================================
/*** BUILDING DATASOURCELIST *************************************************
 * This method builds a DataSourceList from the initial datasource.
 * It tries to find all DICOM files specific to a series UID
 * If one or several of those files doesn't exist, it writes the initial
 * DataSource instead. Thus, the list returned contains at least one ds for
 * each of the following keyword : "dicom", and "default", but
 * in the worst case they can all be the initial ds.
 ****************************************************************************/
Object DicomFormatChecker::_buildDSList( DataSourceList & dsl ) const
{

  DataSource* pds = dsl.dataSource().get();
  string imaname = FileUtil::uriFilename( pds->url() );
  Object hdr = Object::value( PropertySet() );  // header

  if ( !imaname.empty() )
  {

    vector< string > fileList;
    soma::DataInfo dataInfo;

    if ( !soma::DicomIO::getInstance().check( imaname, fileList, dataInfo ) )
    {
      throw wrong_format_error( "Not a DICOM dataset", imaname );
    }

    vector< string >::const_iterator
      f = fileList.begin(),
      fe = fileList.end();

    while ( f != fe )
    {

      dsl.addDataSource( "dicom", 
                         rc_ptr<DataSource>( new FileDataSource( *f ) ) );
      ++f;

    }

    string         type;
    int            sizex = 1, sizey = 1, sizez = 1, sizet = 1;
    vector<float>  vs(4, 1.); // voxel size

    vs[ 0 ] = dataInfo.m_pixelSpacingX;
    vs[ 1 ] = dataInfo.m_pixelSpacingY;
    vs[ 2 ] = dataInfo.m_pixelSpacingZ;
    vs[ 3 ] = dataInfo.m_repetitionTime;

    switch ( dataInfo.m_bpp )
    {

      case 1:
        type = "U8";
        break;

      case 2:
        type = dataInfo.m_pixelRepresentation ? "S16" : "U16";
        break;
        
      case 3:
        type = "RGB";
        break;

      default:
        throw wrong_format_error( "Not a valid DICOM format", 
                                  pds->url() );

    }

    hdr->setProperty( "sizeX", dataInfo.m_width );
    hdr->setProperty( "sizeY", dataInfo.m_height );
    hdr->setProperty( "sizeZ", dataInfo.m_slices );
    hdr->setProperty( "sizeT", dataInfo.m_frames );
    hdr->setProperty( "format", string( "DICOM" ) );
    hdr->setProperty( "voxel_size", vs );
    hdr->setProperty( "object_type", string( "Volume" ) );
    hdr->setProperty( "data_type", type );

  }

  return hdr;

}

//============================================================================
//   P U B L I C   M E T H O D S
//============================================================================

DicomFormatChecker::~DicomFormatChecker()
{
}

DataSourceInfo DicomFormatChecker::check( DataSourceInfo dsi, 
                                          DataSourceInfoLoader & /* f */,
                                          Object /* options */) const
{
  bool doread = dsi.header().isNone() ;
  bool dolist = dsi.list().typecount() == 1 ;
  bool docapa = !dsi.capabilities().isInit();

  //--- read uri -------------------------------------------------------------
  // we don't use any options right now, but we may later ?
  // then, uncomment the following lines to get options from the uri.
//   std::string uri = dsi.list().dataSource( "default", 0 )->url();
//   carto::Object urioptions = FileUtil::uriOptions( uri );
//   if( urioptions.get() ) {
//     options->copyProperties( urioptions );
//   }
  
  //--- test header format ---------------------------------------------------
  if( !doread )
    if( !dsi.header()->hasProperty( "format" ) 
        || dsi.header()->getProperty( "format" )->getString() != "DICOM" )
      throw wrong_format_error( "Not a DICOM header", 
                                dsi.list().dataSource()->url() );

  //--- build datasourcelist -------------------------------------------------
  if( dolist || doread ) {
    localMsg( "Building list and header..." );
    dsi.header() = _buildDSList( dsi.list() );
  }
  //--- build header ---------------------------------------------------------
  //if( doread ) {
  //  localMsg( "Reading header..." );
  //  DataSource* hds = dsi.list().dataSource( "dicom" ).get();
  //  dsi.header() = _buildHeader( hds );
  //}
  //--- write capabilities ---------------------------------------------------
  if( docapa ) {
    localMsg ("Writing capabilities..." );
    dsi.capabilities().setMemoryMapping( false );
    dsi.capabilities().setDataSource( dsi.list().dataSource( "dicom" ) );
    dsi.capabilities().setThreadSafe( true ); /* TODO */
    dsi.capabilities().setOrdered( true );
    dsi.capabilities().setSeekVoxel( false );
    dsi.capabilities().setSeekLine( false );
    dsi.capabilities().setSeekSlice( false );
    dsi.capabilities().setSeekVolume( false );
  }
  //--------------------------------------------------------------------------
  localMsg( "Checking done" );
  return dsi;
}

#undef localMsg