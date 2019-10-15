/* $Id$ */

#ifndef __MGFP_INCLUDE_MGFFILE_H__
#define __MGFP_INCLUDE_MGFFILE_H__

#include "config.h"

#include <ostream>

#include "Collection.h"
#include "MgfHeader.h"
#include "MgfSpectrum.h"

namespace mgf
{

/** A Mascot Generic Format file type. An MgfFile is essentially a container for
 * a set of global settings (i.e. an HgfHeader) and a set of fragment ion
 * spectra, each of which is represented using an MgfSpectrum.
 */
class MGFP_EXPORT MgfFile : public Collection<MgfSpectrum>
{
  public:
    /** Default constructor.
     * Creates an empty MgfFile object.
     */
    MgfFile();
    
    /** Returns the file header information.
     * @param[out] header mgf::MgfHeader object.
     */
    void getHeader(MgfHeader& header);
    
    /** Returns a reference to the file header information.
     * @return Returns a non-const reference to the mgf::MgfHeader object.
     */
    MgfHeader& getHeaderRef();

    /** Set the MGF file header information.
     * @param[in] header mgf::MgfHeader object.
     */
    void setHeader(const MgfHeader& header);

    /** Clear all data from the current MgfFile object.
     */
    void clear();

  private:
    friend std::ostream& operator<<(std::ostream& os, const MgfFile& mgf);
    /** The global MGF parameters
     */
    MgfHeader header_;
};

/** A stream operator to output the contents of an MgfFile object as valid MGF.
 */
std::ostream& operator<<(std::ostream& os, const MgfFile& mgf);

}

#endif

