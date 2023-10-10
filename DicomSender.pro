TEMPLATE = subdirs

SUBDIRS += \
    DicomBase \
    DicomSender

DicomSender.depends = DicomBase
