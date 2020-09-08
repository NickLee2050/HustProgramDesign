#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QVector>
#include <QMainWindow>
#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QPushButton>
#include <QMenuBar>
#include <QMenu>
#include <QDialog>
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    void InitGame(int *puzzle, int *result, int size);
    void SucceedGame();
    ~MainWindow();
public slots:
    void trigerMenu(QAction* act);
private:
    void ElementClicked(int i, int j,int x);
    QVector<QToolButton*> data;
    QVector<int> puzzle;
    QVector<int> result;
    QWidget *widget;
    QGridLayout *layout;
};

#endif // MAINWINDOW_H
