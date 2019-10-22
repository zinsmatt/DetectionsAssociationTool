//=========================================================================
//
// Copyright 2019 Kitware, Inc.
// Author: Matthieu Zins
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <iostream>
#include <filesystem>

#include <QDebug>
#include <QComboBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QLabel>
#include <unsupported/Eigen/EulerAngles>


namespace fs = std::filesystem;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  statusBar()->showMessage("Auto save disabled");

  QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openImagesDataset()));

}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::initialize()
{
}

void MainWindow::update()
{
  std::cout << "update " << index << std::endl;
  cv::Mat dest;
  cv::cvtColor(images[index].image, dest, cv::COLOR_BGR2RGB);
  QImage image_qt= QImage((uchar*) dest.data, dest.cols, dest.rows, dest.step, QImage::Format_RGB888);
  ui->image_view->setPixmap(QPixmap::fromImage(image_qt));
  ui->image_view->show();
  ui->image_label->setText(QString::fromStdString(images[index].name + " (" + std::to_string(index) + ")"));
}

void MainWindow::openImagesDataset()
{
  QFileDialog dialog;
  dialog.setDirectory("/home/matt/dev/MaskRCNN/maskrcnn-benchmark/output/detections");
  dialog.setFileMode(QFileDialog::Directory);
  dialog.exec();
  auto folder = dialog.directory();
  if (folder.exists() && folder.isReadable())
  {
    std::cout << "load images from " << folder.path().toStdString() << std::endl;

    auto path = folder.path().toStdString();
    std::vector<std::string> images_filenames, detections_filenames;
    std::string images_ext;
    for (const auto & entry : fs::directory_iterator(path))
    {
      std::cout << entry.path().filename() << std::endl;
      auto name = entry.path().stem();
      auto ext = entry.path().extension();
      if (ext.generic_string() == ".png" || ext.generic_string() == ".jpg" || ext.generic_string() == ".jpeg")
      {
        images_filenames.push_back(name.generic_string());
        images_ext = ext.generic_string();
      }
      else if(ext.generic_string() == ".txt")
      {
        detections_filenames.push_back(name.generic_string());
      }
    }
    std::sort(detections_filenames.begin(), detections_filenames.end());
    std::sort(images_filenames.begin(), images_filenames.end());

    for (int i = 0; i < detections_filenames.size(); ++i)
    {
      if (detections_filenames[i] != images_filenames[i])
      {
        std::cerr << "Warning: some images / detections do not match.";
        return;
      }
    }

    // Ask to select a subset of images

    auto line = QInputDialog::getText(nullptr, "Select images to use", "Indices: ",
                                      QLineEdit::Normal, "");
    std::cout << line.toStdString() << "\n";
    auto lineStr = line.toStdString();
    for (auto& c  : lineStr)
    {
      if (c == ',')
        c = ' ';
    }

    std::vector<int> indices;
    if (lineStr[0] == '*')
    {
      indices.resize(images_filenames.size());
      std::iota(indices.begin(), indices.end(), 0);
    }
    else
    {
      std::stringstream ss;
      ss.str(lineStr);
      int x;
      while (ss >> x)
      {
        if (x < 0 || x >= images_filenames.size())
        {
          std::cerr << "Warning: invalid index " << x << std::endl;
          return;
        }
        indices.push_back(x);
      }
    }

    // Load the images and detections
    for (auto idx : indices)
    {
      auto image_fullpath = fs::path(folder.path().toStdString()) / fs::path(images_filenames[idx] + images_ext);
      auto detection_fullpath = fs::path(folder.path().toStdString()) / fs::path(detections_filenames[idx] + ".txt");
      Image image(image_fullpath);
      image.loadDetectionsFile(detection_fullpath.generic_string());
      images.emplace_back(image);
    }

    if (images.size() > 0)
    {
      index = 0;
    }
    update();
  }
}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
  if (images.size() > 0)
  {
    if (ev->key() == 16777236)  // next
    {
      index = (index + 1) % images.size();
    }
    else if (ev->key() == 16777234)
    {
      index -= 1;
      if (index < 0)
        index += images.size();
    }
    update();
  }
}

