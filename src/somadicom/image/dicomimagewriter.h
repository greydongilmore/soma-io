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

#ifndef SOMAIO_IMAGE_DICOMIMAGEWRITER_H
#define SOMAIO_IMAGE_DICOMIMAGEWRITER_H
//--- soma-io ----------------------------------------------------------------
#include <soma-io/config/soma_config.h>
#include <soma-io/image/imagewriter.h>
//--- cartobase --------------------------------------------------------------
#include <cartobase/object/object.h>                        // header, options
//--- system -----------------------------------------------------------------
#include <vector>
//----------------------------------------------------------------------------

namespace soma
{
  class DataSourceInfo;

  /// abstract ImageWriter for DICOM files.
  template<typename T>
  class DicomImageWriter : public ImageWriter<T>
  {
    public:
      //======================================================================
      //   C O N S T R U C T O R S
      //======================================================================
      DicomImageWriter();
      virtual ~DicomImageWriter();

      //======================================================================
      //   I M A G E W R I T E R
      //======================================================================
      virtual void write( const T * source, DataSourceInfo & dsi,
        const std::vector<int> & pos,  /* size 4 : x,y,z,t */
        const std::vector<int> & size, /* size 4 : x,y,z,t */
        const std::vector<long> & strides, /* size 4 : x,y,z,t */
        carto::Object options = carto::none() );

      virtual DataSourceInfo writeHeader( DataSourceInfo & dsi, 
                                          const T * source,
                                          const std::vector<int> & pos,
                                          const std::vector<int> & size,
                                          const std::vector<long> & strides,
                                          carto::Object options
                                            = carto::none() );

      virtual void updateParams( DataSourceInfo & dsi );
      virtual void resetParams();

    protected:
      void buildDSList( DataSourceList & dsl ) const;

      std::vector<std::vector<int> >  _sizes;
      std::vector<float> _voxel_size;
  };

}

#endif
