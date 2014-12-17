#include <soma-io/Dicom/CTImageStorageReader.h>
#include <soma-io/Dicom/DicomDatasetHeader.h>
#include <soma-io/Container/DicomProxy.h>
#include <soma-io/Object/HeaderProxy.h>
#include <soma-io/Pattern/Callback.h>
#include <soma-io/Dicom/DicomDataContext.h>
#include <cartobase/thread/threadedLoop.h>
#include <soma-io/Utils/StdInt.h>

#include <soma-io/Dicom/soma_osconfig.h>
#include <dcmtk/dcmdata/dcdatset.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcuid.h>


soma::CTImageStorageReader::CTImageStorageReader()
                          : soma::MultiFileReader()
{
}


std::string soma::CTImageStorageReader::getStorageUID()
{

  return UID_CTImageStorage;

}


bool soma::CTImageStorageReader::getHeader( 
                                       soma::HeaderProxy& header, 
                                       soma::DataInfo& info,
                                       soma::DicomDatasetHeader& datasetHeader )
{

  if ( !soma::MultiSliceReader::getHeader( header, info, datasetHeader ) )
  {

    return false;

  }

  Float64 tmpDouble;
  DcmDataset dataset;

  datasetHeader.get( dataset );

  if ( dataset.findAndGetFloat64( DCM_ReconstructionDiameter, 
                                  tmpDouble ).good() )
  {

    header.addAttribute( "reconstruction_diameter", double( tmpDouble ) );

  }

  return true;

}


bool soma::CTImageStorageReader::readHeader( DcmDataset* dataset )
{

  m_dataInfo->m_slices = int32_t( m_slices.size() );

  return soma::MultiFileReader::readHeader( dataset );

}


bool soma::CTImageStorageReader::readData( soma::DicomProxy& proxy,
                                           soma::Callback* progress )
{

  if ( proxy.allocate() )
  {

    soma::DicomDataContext context( m_slices, 
                                    proxy, 
                                    false,
                                    progress );
    carto::ThreadedLoop threadedLoop( &context,
                                      0,
                                      int32_t( m_slices.size() ) );

    threadedLoop.launch();

    return true;

  }

  return false;

}
