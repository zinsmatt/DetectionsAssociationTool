//=========================================================================
//
// Copyright 2019
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

#include <yaml-cpp/yaml.h>

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
    cv::ellipse(dest, cv::RotatedRect(cv::Point2i(obs->x, obs->y), cv::Size2i(obs->w, obs->h), 180.0 * obs->a / 3.14156),
                cv::Scalar(0, 255, 0), 3);
  }

  QImage image_qt= QImage((uchar*) dest.data, dest.cols, dest.rows, dest.step, QImage::Format_RGB888);
  ui->image_view->setPixmap(QPixmap::fromImage(image_qt));
  ui->image_view->show();

  QFont font( "Verdana", 20, QFont::Bold);
  ui->image_label->setFont(font);
  ui->image_label->setText(QString::fromStdString(std::to_string(cur_image_index)));

  QFont font2( "Verdana", 10);
  ui->image_caption->setFont(font2);
  ui->image_caption->setText(QString::fromStdString(images[cur_image_index].getText()));

  ui->image_view->setMainWindow(this);
  ui->image_view->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
  ui->image_view->setScaledContents(false);
}

void MainWindow::click(int x, int y, int mode)
{
    if (ui->image_view->pixmap())
    {
      if (mode == 0)
      {
          int x_offset = static_cast<float>(ui->image_view->width() - ui->image_view->pixmap()->width()) / 2;
          int y_offset = static_cast<float>(ui->image_view->height() - ui->image_view->pixmap()->height()) / 2;
          if (cur_obj_index >= 0)
          {
            const auto* det = images[cur_image_index].findDetectionUnderPosition(x - x_offset, y - y_offset);
            if (det)
            {
              objects[cur_obj_index].setObservation(cur_image_index, det);
              update();
            }
          }
      }
      else if (mode == 1)
      {
          std::cout << "remove observation" << std::endl;
          objects[cur_obj_index].removeObservation(cur_image_index);
          update();
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
    /// This function opens a detection dataset. This should at least contain:
    ///     *.ellipses => used to display the ellipse when the user click on a detection
    ///     images/ *.png => the images with the detections that are displayed

  QFileDialog dialog;
//  dialog.setDirectory("/home/matt/dev/MaskRCNN/maskrcnn-benchmark/output/detections");
  dialog.setDirectory("/home/mzins/Dataset/rgbd_dataset_freiburg2_desk/MaskRCNN_detections");
  dialog.setFileMode(QFileDialog::Directory);
  int ret = dialog.exec();
  auto folder = dialog.directory();
  if (ret && folder.path().toStdString().size() > 0 && folder.exists() && folder.isReadable())
  {
    clear();

    fs::path data_path(folder.path().toStdString());
    fs::path images_path = data_path / "images";
    std::vector<std::string> images_filenames, detections_filenames, classes_filenames;
    std::string images_ext;
    for (const auto & entry : fs::directory_iterator(images_path))
    {
      auto name = entry.path().stem();
      auto ext = entry.path().extension();
      if (ext.generic_string() == ".png" || ext.generic_string() == ".jpg" || ext.generic_string() == ".jpeg")
      {
        images_filenames.push_back(name.generic_string());
        images_ext = ext.generic_string();
      }
    }
    for (const auto & entry : fs::directory_iterator(data_path))
    {
      auto name = entry.path().stem();
      auto ext = entry.path().extension();
      if(ext.generic_string() == ".ellipses")
      {
        detections_filenames.push_back(name.generic_string());
      }
      else if (ext.generic_string() == ".classes")
      {
          classes_filenames.push_back(name.generic_string());
      }
    }

    std::sort(detections_filenames.begin(), detections_filenames.end());
    std::sort(images_filenames.begin(), images_filenames.end());
    std::sort(classes_filenames.begin(), classes_filenames.end());

    if (detections_filenames != images_filenames || (classes_filenames.size() > 0 && detections_filenames != classes_filenames))
    {
      std::cerr << "Warning: some images / detections do not match.";
      return;
    }

    // Ask to select a subset of images
    auto line = QInputDialog::getText(nullptr, "Select images to use", "Indices: ",
                                      QLineEdit::Normal, "");
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
      auto image_fullpath = images_path / fs::path(images_filenames[idx] + images_ext);
      auto detection_fullpath = data_path / fs::path(images_filenames[idx] + ".ellipses");
      auto classe_fullpath = data_path / fs::path(images_filenames[idx] + ".classes");
      Image image(image_fullpath, idx);
      if (classes_filenames.size() > 0)
          image.loadImageDetections(detection_fullpath.generic_string(), classe_fullpath.generic_string());
      else
          image.loadImageDetections(detection_fullpath.generic_string());
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
    else if (ev->key() == 16777223)         // "Suppr" => remove the currently selected object
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
            objectCounter--;
        }
    }
    else if (ev->key() == 16777216)         // "Esc" => unselect objects
    {
        cur_obj_index = -1;
        ui->objects_list->setCurrentRow(-1);
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

bool MainWindow::save()
{
    /// It writes three files: .associations, .txt, .used_images.txt
    ///     .txt contains a large matrix where each 3x3 matrix is an ellipse (dual form). The columns represent the different objects
    ///     .associations contains first the number and the list of all images and their index in the dataset. Then, the number of objects
    ///     and for each object their id and their observation as an ellipse in each image (-1 -1 -1 -1 -1 if no observation in the image)
    ///     .used_images.txt just contains the name of each image
    ///     .ellipses_associations
    ///
    auto outputFilename = QFileDialog::getSaveFileName(this, tr("Save the detections associations"), "./", tr("YAML File (*.yaml, *.yml)")).toStdString();
    if (outputFilename.size() > 0)
    {
        YAML::Node data_node;
        YAML::Node images_node;
        fs::path images_folder = fs::path(images.front().fullpath).parent_path();
        images_node["folder"] = images_folder.generic_string();
        for (const auto& im : images)
        {
            images_node["names"].push_back(im.name);
            images_node["ids"].push_back(im.dataset_index);
        }

        data_node["images"] = images_node;
        data_node["detections_folder"] =images_folder.parent_path().generic_string();

        for (const auto& obj : objects)
        {
            YAML::Node object_node;
            for (int i = 0; i < images.size(); ++i)
            {
                const auto* obs = obj.getObservation(i);
                if (obs)
                {
                    std::vector<double> ellipse = {obs->w / 2, obs->h / 2, obs->x, obs->y, obs->a};
    //                object_node["ellipses"].push_back(ellipse);
                    object_node["dataset_img_ids"].push_back(images[i].dataset_index);
                    object_node["short_img_ids"].push_back(i);
                    object_node["obs_ids"].push_back(obs->in_image_index);
    //                object_node["classes"].push_back(obs->classe);
    //                object_node["classes_names"].push_back(obs->classe_name);
                }
            }
            data_node["objects"].push_back(object_node);
        }
        std::ofstream fileo("test.yaml");
        fileo << data_node;
        fileo.close();

        return true;
    }
    return false;
}

void MainWindow::load()
{
    /// this functions reload the necessary data (images + detections) to be able to continue editing an association file
    ///

    auto datasetFilename = QFileDialog::getOpenFileName(this, tr("Open the detections associations"), "./", tr("YAML File (*.yaml, *.yml)")).toStdString();
    if (datasetFilename.size() > 0)
    {
        // CLEAR
        clear();


        auto data = YAML::LoadFile("/home/mzins/dev/DetectionsAssociationTool/build/test.yaml");
        fs::path folder = data["images"]["folder"].as<std::string>();
        auto const& names = data["images"]["names"].as<std::vector<std::string>>();
        auto const& ids = data["images"]["ids"].as<std::vector<int>>();
        const int n_images = names.size();
        for (int i = 0; i < n_images; ++i)
        {
            fs::path full_path = folder / fs::path(names[i]);
            Image image(full_path.generic_string() , ids[i]);
            fs::path detections_path = full_path;
            detections_path.replace_extension(".ellipses");
            fs::path classes_path = full_path;
            classes_path.replace_extension(".classes");
            full_path.replace_extension(".classes");
            image.loadImageDetections(detections_path.generic_string(), classes_path.generic_string());
            images.emplace_back(image);
        }

        auto const& objects_node = data["objects"];
        const int  n_objects = objects_node.size();
        for (int i = 0; i < n_objects; ++i)
        {
            int id = objects.size();
            Object obj(id, n_images);

            auto const& object_node = objects_node[i];
//            auto const& ellipses = object_node["ellipses"].as<std::vector<std::vector<double>>>();
//            auto classes = object_node["classes"].as<std::vector<int>>();
//            auto classes_names = object_node["classes_names"].as<std::vector<std::string>>();
            auto img_ids = object_node["dataset_img_ids"].as<std::vector<int>>();
            auto short_img_ids = object_node["short_img_ids"].as<std::vector<int>>();
            auto obs_ids = object_node["obs_ids"].as<std::vector<int>>();

            for (int j = 0; j < obs_ids.size(); ++j)
            {
                obj.setObservation(short_img_ids[j], &(images[short_img_ids[j]].detections[obs_ids[j]]));
            }
            objects.emplace_back(std::move(obj));
            ui->objects_list->addItem(obj.getText());
        }

        objectCounter = n_objects;
        cur_image_index = 0;

        std::cout << "Successfully loaded association dataset" << std::endl;
        update();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (objects.size() > 0)
  {
    QMessageBox::StandardButton ret = QMessageBox::warning(this, "Warning",
                 tr("Do you want to save your work ?\n"),
                 QMessageBox::Discard | QMessageBox::Save| QMessageBox::Cancel,
                 QMessageBox::Discard);
    if (ret == QMessageBox::Save)
    {
        if (save())
        {   // if saved successfully, we can quit
            event->accept();
        }
        else
        {   // if no saving, ignore the action
            event->ignore();
        }
    }
    else if (ret == QMessageBox::Cancel)
    {
      event->ignore();
    }
    else
    {
      event->accept();
    }
  }
}
