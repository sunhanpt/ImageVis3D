/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2008 Scientific Computing and Imaging Institute,
   University of Utah.

   
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/


//!    File   : ImageVis3D_Capturing.cpp
//!    Author : Jens Krueger
//!             SCI Institute
//!             University of Utah
//!    Date   : September 2008
//
//!    Copyright (C) 2008 SCI Institute

#include "ImageVis3D.h"
#include "../Tuvok/Basics/SysTools.h"
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>
#include <QtGui/QInputDialog>
#include <QtCore/QSettings>

#include "PleaseWait.h"
#include "DebugOut/QTLabelOut.h"
#include "../Tuvok/DebugOut/MultiplexOut.h"

using namespace std;

void MainWindow::CaptureFrame() {
  if (m_ActiveRenderWin) {
    if (!m_ActiveRenderWin->CaptureFrame(lineEditCaptureFile->text().toStdString())) {
    QString msg = tr("Error writing image file %1").arg(lineEditCaptureFile->text());
    QMessageBox::warning(this, tr("Error"), msg);
    }
  }
}

void MainWindow::CaptureSequence() {
  if (m_ActiveRenderWin) {
    if (!m_ActiveRenderWin->CaptureSequenceFrame(lineEditCaptureFile->text().toStdString())) {
    QString msg = tr("Error writing image file %1").arg(lineEditCaptureFile->text());
    QMessageBox::warning(this, tr("Error"), msg);
    }
  }
}

void MainWindow::CaptureRotation() {
  if (m_ActiveRenderWin) {

    QSettings settings;
    int iNumImages = settings.value("Renderer/ImagesPerRotation", 360).toInt();

    bool ok;
    iNumImages = QInputDialog::getInteger(this,
                                          tr("How many images to you want to compute?"),
                                          tr("How many images to you want to compute:"), iNumImages, 1, 3600, 1, &ok);

    if (!ok) return;

    settings.setValue("Renderer/ImagesPerRotation", iNumImages);


    AbstrRenderer::EWindowMode eWindowMode = m_ActiveRenderWin->GetRenderer()->GetFullWindowmode();

    m_ActiveRenderWin->ToggleHQCaptureMode();
    
    PleaseWaitDialog pleaseWait(this);
    // add status label into debug chain
    AbstrDebugOut* pOldDebug       = m_MasterController.DebugOut();
    MultiplexOut* pMultiOut = new MultiplexOut();
    m_MasterController.SetDebugOut(pMultiOut, true);
    QTLabelOut* labelOut = new QTLabelOut(pleaseWait.GetStatusLabel());
    labelOut->m_bShowMessages = true;
    labelOut->m_bShowWarnings = true;
    labelOut->m_bShowErrors = true;
    labelOut->m_bShowOther = false;
    pMultiOut->AddDebugOut(labelOut,  true);
    pMultiOut->AddDebugOut(pOldDebug, false);

    if (eWindowMode == AbstrRenderer::WM_3D)  {
      pleaseWait.SetText("Capturing a full 360� rotation, please wait  ...");
     
      int i = 0;
      float fAngle = 0.0f;
      while (i < iNumImages) {
        labelOut->m_bShowMessages = true;
        labelOut->m_bShowWarnings = true;
        labelOut->m_bShowErrors = true;
        if (i==0)
          m_MasterController.DebugOut()->Message("MainWindow::CaptureRotation", "Processing Image %i of %i (the first image may be slower due to caching)\n%i percent completed",i+1,iNumImages,int(100*float(i)/float(iNumImages)) );
        else
          m_MasterController.DebugOut()->Message("MainWindow::CaptureRotation", "Processing Image %i of %i\n%i percent completed",i+1,iNumImages,int(100*float(i)/float(iNumImages)) );
        labelOut->m_bShowMessages = false;
        labelOut->m_bShowWarnings = false;
        labelOut->m_bShowErrors = false;
        fAngle = float(i) * 360.0f/float(iNumImages);
        m_ActiveRenderWin->SetCaptureRotationAngle(fAngle);
        if (!m_ActiveRenderWin->CaptureSequenceFrame(lineEditCaptureFile->text().toStdString())) {
          QString msg = tr("Error writing image file %1").arg(lineEditCaptureFile->text());
          QMessageBox::warning(this, tr("Error"), msg);
          break;
        }
        i++;
      }
    } else {
      if (m_ActiveRenderWin->GetRenderer()->GetUseMIP(eWindowMode))  {

        pleaseWait.SetText("Capturing a full 360� MIP rotation, please wait  ...");
        int i = 0;
        float fAngle = 0.0f;
        while (i < iNumImages) {
          labelOut->m_bShowMessages = true;
          labelOut->m_bShowWarnings = true;
          labelOut->m_bShowErrors = true;
          if (i==0)
            m_MasterController.DebugOut()->Message("MainWindow::CaptureRotation", "Processing Image %i of %i (the first image may be slower due to caching)\n%i percent completed",i+1,iNumImages,int(100*float(i)/float(iNumImages)) );
          else
            m_MasterController.DebugOut()->Message("MainWindow::CaptureRotation", "Processing Image %i of %i\n%i percent completed",i+1,iNumImages,int(100*float(i)/float(iNumImages)) );
          labelOut->m_bShowMessages = false;
          labelOut->m_bShowWarnings = false;
          labelOut->m_bShowErrors = false;

          fAngle = float(i) * 360.0f/float(iNumImages);

          if (!m_ActiveRenderWin->CaptureMIPFrame(lineEditCaptureFile->text().toStdString(), fAngle)) {
            QString msg = tr("Error writing image file %1.").arg(lineEditCaptureFile->text());
            QMessageBox::warning(this, tr("Error"), msg);
            break;
          }
          i++;
        }                   
      } else {
        pleaseWait.SetText("Slicing trougth the dataset, please wait  ...");
        /// \todo TODO slice capturing
        QString msg = tr("Slice Capturing is not implemented yet. Aborting.");
        QMessageBox::warning(this, tr("Error"), msg);
      }
    }
    m_ActiveRenderWin->ToggleHQCaptureMode();
    pleaseWait.close();
    m_MasterController.SetDebugOut(pOldDebug);
    m_ActiveRenderWin->GetRenderer()->ScheduleCompleteRedraw();  // to make sure front and backbuffer are valid
  }
}

void MainWindow::SetCaptureFilename() {
  QFileDialog::Options options;
#if defined(macintosh) || (defined(__MACH__) && defined(__APPLE__))
  options |= QFileDialog::DontUseNativeDialog;
#endif
  QString selectedFilter;

  QSettings settings;
  QString strLastDir = settings.value("Folders/SetCaptureFilename", ".").toString();

  QString fileName = QFileDialog::getSaveFileName(this,"Select Image File", strLastDir,
             "All Files (*.*)",&selectedFilter, options);


  if (!fileName.isEmpty()) {
    // add png as the default filetype if the user forgot to enter one
    if (SysTools::GetExt(fileName.toStdString()) == "")
      fileName = fileName + ".png";

    settings.setValue("Folders/SetCaptureFilename", QFileInfo(fileName).absoluteDir().path());
    settings.setValue("Files/SetCaptureFilename", fileName);
    lineEditCaptureFile->setText(fileName);
  }

}
