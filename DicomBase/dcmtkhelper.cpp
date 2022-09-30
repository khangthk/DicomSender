#include "dcmtkhelper.h"

#include <dcmtk/dcmdata/dcfilefo.h>

bool DcmtkHelper::isDicom(const QString &file)
{
    DcmFileFormat fileformat;
    OFCondition status = fileformat.loadFile(qPrintable(file), EXS_Unknown, EGL_noChange, DCM_MaxReadLength, ERM_fileOnly);
    return status.good();
}
