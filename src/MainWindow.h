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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTimer>
#include <QCloseEvent>
#include <QComboBox>
#include <QListWidgetItem>

#include "detection.h"
#include "image.h"
#include "object.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT


public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void update();

  // mode is 0 for select, 1 for delete
  void click(int x, int y, int mode);

  void closeEvent(QCloseEvent *event) override;

public slots:
  void openImagesDataset();
  void keyPressEvent(QKeyEvent *ev) override;
  void addNewObject();
  void selectObject();
  bool save();
  void load();
  void clear();


private:
  Ui::MainWindow *ui;

  std::vector<Image> images;

  int cur_image_index = -1;
  int cur_obj_index = -1;

  std::vector<Object> objects;

  int objectCounter = 0;

};

#endif // MAINWINDOW_H
