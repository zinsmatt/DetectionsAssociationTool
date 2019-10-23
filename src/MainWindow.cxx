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
#include <experimental/filesystem>

#include <QDebug>
#include <QComboBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QLabel>
#include <QListWidget>
#include <unsupported/Eigen/EulerAngles>


namespace fs = std::experimental::filesystem;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent), ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  statusBar()->showMessage("Auto save disabled");

  QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openImagesDataset()));
  QObject::connect(ui->add_object_button, SIGNAL(clicked(bool)), this, SLOT(addNewObject()));
  QObject::connect(ui->objects_list, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectObject(QListWidgetItem*)));
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::update()
{
  if (cur_image_index < 0)
      return;

  const Detection* obs = nullptr;
  if (cur_obj_index >= 0)
    obs = objects[cur_obj_index].getObservation(cur_image_index);

  cv::Mat dest;
  cv::cvtColor(images[cur_image_index].image, dest, cv::COLOR_BGR2RGB);
  if (obs)
  {
    cv::ellipse(dest, cv::RotatedRect(cv::Point2i(obs->x, obs->y), cv::Size2i(obs->w, obs->h), obs->a),
                cv::Scalar(0, 255, 0), 3);
  }
  QImage image_qt= QImage((uchar*) dest.data, dest.cols, dest.rows, dest.step, QImage::Format_RGB888);
  ui->image_view->setPixmap(QPixmap::fromImage(image_qt));
  ui->image_view->show();
  ui->image_label->setText(QString::fromStdString(std::to_string(cur_image_index) + "\t" + images[cur_image_index].getText()));

  ui->image_view->setMainWindow(this);
  ui->image_label->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  ui->image_label->setScaledContents(false);
}

void MainWindow::click(int x, int y)
{
  if (cur_obj_index >= 0)
  {
    const auto* det = images[cur_image_index].findDetectionUnderPosition(x, y);
    if (det)
    {
      objects[cur_obj_index].setObservation(cur_image_index, *det);
      update();
    }
  }
}

void MainWindow::openImagesDataset()
{
  QFileDialog dialog;
//  dialog.setDirectory("/home/matt/dev/MaskRCNN/maskrcnn-benchmark/output/detections");
  dialog.setDirectory("/home/mzins/Dataset/rgbd_dataset_freiburg2_desk/MaskRCNN_detections");
  dialog.setFileMode(QFileDialog::Directory);
  int ret = dialog.exec();
  auto folder = dialog.directory();
  if (ret && folder.path().toStdString().size() > 0 && folder.exists() && folder.isReadable())
  {
    std::cout << "load images from " << folder.path().toStdString() << std::endl;

    auto path = folder.path().toStdString();
    std::vector<std::string> images_filenames, detections_filenames;
    std::string images_ext;
    for (const auto & entry : fs::directory_iterator(path))
    {
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
    std::cout << "line = " << line.toStdString() << "\n";
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
      Image image(image_fullpath, idx);
      image.loadDetectionsFile(detection_fullpath.generic_string());
      images.emplace_back(image);
    }

    if (images.size() > 0)
    {
      cur_image_index = 0;
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
      cur_image_index = (cur_image_index + 1) % images.size();
    }
    else if (ev->key() == 16777234)
    {
      cur_image_index -= 1;
      if (cur_image_index < 0)
        cur_image_index += images.size();
    }
    update();
  }
}

void MainWindow::addNewObject()
{
  objects.emplace_back(Object(objectCounter++, images.size()));
  ui->objects_list->addItem(objects.back().getText());
}

void MainWindow::selectObject(QListWidgetItem *item)
{
  auto name = item->text().toStdString();
  auto num = std::stoi(name.substr(7));
  cur_obj_index = num;
  std::cout << "selected object " << num << std::endl;
}

