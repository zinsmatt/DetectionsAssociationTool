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

#include <iomanip>
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

  QObject::connect(ui->actionOpenDetections, SIGNAL(triggered()), this, SLOT(openImagesDataset()));
  QObject::connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
  QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(load()));
  QObject::connect(ui->actionRestart, SIGNAL(triggered()), this, SLOT(clear()));
  QObject::connect(ui->add_object_button, SIGNAL(clicked(bool)), this, SLOT(addNewObject()));
  QObject::connect(ui->objects_list, SIGNAL(itemSelectionChanged()), this, SLOT(selectObject()));

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
    std::cout << "lickec" << std::endl;
    if (ui->image_view->pixmap())
    {
      int x_offset = static_cast<float>(ui->image_view->width() - ui->image_view->pixmap()->width()) / 2;
      int y_offset = static_cast<float>(ui->image_view->height() - ui->image_view->pixmap()->height()) / 2;
      std::cout << ")=====> fofsets " << x_offset << " " << y_offset << std::endl;
      if (cur_obj_index >= 0)
      {
        const auto* det = images[cur_image_index].findDetectionUnderPosition(x - x_offset, y - y_offset);
        if (det)
        {
          objects[cur_obj_index].setObservation(cur_image_index, *det);
          update();
        }
      }
    }
}

void MainWindow::clear()
{
    images.clear();
    objects.clear();
    cur_image_index = -1;
    cur_obj_index = -1;
    objectCounter = 0;
    ui->objects_list->clear();
    ui->image_label->clear();
    ui->image_view->clear();
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
    clear();

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
    std::cout << "key = " << ev->key() << std::endl;
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
    else if (ev->key() == 16777223)
    {
        if (cur_obj_index >= 0)
        {
            for (int i = cur_obj_index + 1; i < objects.size(); ++i)
            {
                objects[i].setId(objects[i].getId() - 1);
                ui->objects_list->item(i)->setText(QString::fromStdString("Object " + std::to_string(objects[i].getId())));
            }

            objects.erase(objects.begin() + cur_obj_index);
            QListWidgetItem *it = ui->objects_list->takeItem(cur_obj_index);
            delete it;

            cur_obj_index = -1;
            ui->objects_list->setCurrentRow(-1);
            update();
        }
    }
    update();
  }
}

void MainWindow::addNewObject()
{
  objects.emplace_back(Object(objectCounter++, images.size()));
  ui->objects_list->addItem(objects.back().getText());
  cur_obj_index = objects.size() - 1;
  ui->objects_list->setCurrentRow(cur_obj_index);
  update();
}

void MainWindow::selectObject()
{
  QListWidgetItem* item = nullptr;
  for (int i = 0; i < ui->objects_list->count(); ++i)
  {
      if (ui->objects_list->item(i)->isSelected())
      {
          item = ui->objects_list->item(i);
      }
  }
  if (item)
  {
    auto name = item->text().toStdString();
    auto num = std::stoi(name.substr(7));
    cur_obj_index = num;
    update();
  }
}

void MainWindow::save()
{
    auto outputFilename = QFileDialog::getSaveFileName(this, tr("Save the detections associations"), "./", tr("Text File (*.txt)")).toStdString();
    if (outputFilename.size() > 0)
    {
        // force the extension
        fs::path filename(outputFilename);
        filename.replace_extension("txt");
        outputFilename = filename.generic_string();

        filename.replace_extension(".associations");
        auto outputFilename3 = filename.generic_string();

        filename.replace_extension("used_images.txt");
        auto outputFilename2 = filename.generic_string();


        Eigen::MatrixXd matrices(images.size() * 3, 0);
        for (const auto& obj : objects)
        {
            Eigen::MatrixX3d ellipsoids_matrices = obj.getObservationsMatrix();
            matrices.conservativeResize(Eigen::NoChange, matrices.cols() + 3);
            std::cout << "*-------------------------------------------------------------------*\n";
            std::cout << "matrices = \n" << matrices << std::endl;
            std::cout << "*-------------------------------------------------------------------*\n";
            std::cout << "ellipsoids_matrices = \n" << ellipsoids_matrices << std::endl;
            matrices.rightCols(3) = ellipsoids_matrices;
            std::cout << "*-------------------------------------------------------------------*\n";
            std::cout << "matrices = \n" << matrices << std::endl;
        }

        std::ofstream file(outputFilename);
        for (int i = 0; i < matrices.rows(); ++i)
        {
            for (int j = 0; j < matrices.cols(); ++j)
            {
                file << std::fixed << std::setprecision(9) << std::setfill(' ') << std::setw(20) <<  matrices(i, j) << " ";
            }
            file << "\n";
        }

        file.close();

        std::ofstream file2(outputFilename2);
        for (const auto& img : images)
            file2 << img.name << "\n";
        file2.close();

        std::ofstream file3(outputFilename3);
        file3 << images.size() << "\n";
        for (const auto& img : images)
            file3 << img.fullpath << " " << img.dataset_index << "\n";
        file3 << objects.size() << "\n";
        for (const auto& obj : objects)
        {
            file3 << obj.serialize();
        }
        file3.close();
    }
}

void MainWindow::load()
{
    auto datasetFilename = QFileDialog::getOpenFileName(this, tr("Open the detections associations"), "./", tr("Associations File (*.associations)")).toStdString();
    if (datasetFilename.size() > 0)
    {

        // CLEAR
        clear();

        std::ifstream file(datasetFilename);
        int n_images;
        file >> n_images;
        for (int i = 0; i < n_images; ++i)
        {
            std::string filename;
            int idx;
            file >> filename >> idx;
            Image image(filename, idx);
            fs::path detections_fullpath(filename);
            detections_fullpath.replace_extension(".txt");
            image.loadDetectionsFile(detections_fullpath.generic_string());
            images.emplace_back(image);
        }

        int n_objects;
        file >> n_objects;
        for (int i = 0; i < n_objects; ++i)
        {
            int id;
            file >> id;
            Object obj(id, n_images);
            for (int j = 0; j < n_images; ++j)
            {
                float x, y, w, h, a;
                int classe;
                file >> x >> y >> w >> h >> a >> classe;
                if (x > 0 && y > 0)
                {
                    Detection det({x, y, w, h, a, classe});
                    obj.setObservation(j, det);
                }
            }
            objects.emplace_back(std::move(obj));
            ui->objects_list->addItem(obj.getText());
        }
        objectCounter = n_objects;
        file.close();

        std::cout << "Successfully loaded association dataset" << std::endl;

        cur_image_index = 0;
        update();
    }
}
