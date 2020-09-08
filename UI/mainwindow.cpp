#include "mainwindow.h"
#include "GenBoard.h"
#include "ConvertBoard.h"
#include "DPLLv3.3.h"
//构造函数，窗口对象初始化会执行
static ClaNode* h = NULL;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    widget = new QWidget(this);
    this->setCentralWidget(widget);
    QMenuBar *bar = new QMenuBar();
    QMenu *Game = bar->addMenu("Game");
    QMenu *CNF_Solve = bar->addMenu("Solve_CNF");
    Game->addAction("New Game");
    Game->addAction("Save as CNF");
    Game->addAction("Rules");
    Game->addAction("About");
    CNF_Solve->addAction("Choose File...");
    connect(bar, SIGNAL(triggered(QAction *)), this, SLOT(trigerMenu(QAction *)));
    this->setMenuBar(bar);
    layout = NULL;
    QMessageBox::information(NULL, "Welcome", "Welcome to the Binary Puzzle Game\t\n\n"
                                              "\"Binary\" means you can only put \"1\" or \"0\" in a box.\n\n"
                                              "More than that, there are rules;\n"
                                              "Rule 1: No more than two similar numbers next to each other horizontally and vertically.\n"
                                              "Rule 2: Each row and each column should contain an equal number of zeros and ones.\n"
                                              "Rule 3: No same rows and same columns.\n\n"
                                              "You can check the rules anytime from the menu:\n"
                                              "Game->Rules\n\n"
                                              "To start a game, please turn to:\n"
                                              "Game->New Game",
                             QMessageBox::Yes, QMessageBox::Yes);
    QMessageBox::information(NULL, "Notice", "This program applies DPLL, optimized by CDCL "
                                             "to solve SAT problems decribed in CNF format.\n\n"
                                              "You can export generated puzzles as SAT problems in CNF by clicking:\n"
                                              "Game->Save as CNF\n\n"
                                             "You can also solve a CNF file with the built-in algorithm by clicking:\n"
                                             "Solve_CNF->Choose File...\n\n"
                                             "Good luck! & Have fun!\n",
                             QMessageBox::Yes, QMessageBox::Yes);

}
void MainWindow::InitGame(int *puz, int *res, int size)
{
    //启动每一场游戏时执行,绘制游戏界面
    for (int k = 0; k < data.length(); k++)
    {
        delete (data[k]);
    }
    data.clear();
    if (layout != NULL)
    {
        delete (layout);
    }
    layout = new QGridLayout();
    puzzle.clear();
    result.clear();
    for (int i = 1; i <= size * size; i++)
    {
        puzzle.append(puz[i]);
        result.append(res[i]);
    }
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            QToolButton *edit = new QToolButton(this);
            data.append(edit);
            edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            if (puzzle[i * size + j] == -1)
            {
                edit->setEnabled(false);
                edit->setText("0");
            }
            else if (puzzle[i * size + j] == 1)
            {
                edit->setEnabled(false);
                edit->setText("1");
            }
            connect(edit, &QToolButton::clicked, [this, i, j, size]() {
                ElementClicked(i, j, size);
            });
            layout->addWidget(edit, i, j);
        }
    }
    widget->setLayout(layout);
}

void MainWindow::ElementClicked(int i, int j, int x)
{
    QFont font;
    if (puzzle[i * x + j] == 0)
    {
        puzzle[i * x + j] = 1;
        data[i * x + j]->setText("1");
        if (puzzle[i * x + j] != result[i * x + j])
            data[i * x + j]->setStyleSheet("color:red;");
        else
            data[i * x + j]->setStyleSheet("color:black;");
    }
    else if (puzzle[i * x + j] == 1)
    {
        puzzle[i * x + j] = -1;
        data[i * x + j]->setText("0");
        if (puzzle[i * x + j] != result[i * x + j])
            data[i * x + j]->setStyleSheet("color:red;");
        else
            data[i * x + j]->setStyleSheet("color:black;");
    }
    else
    {
        puzzle[i * x + j] = 0;
        data[i * x + j]->setText(" ");
    }
    SucceedGame();
};
//菜单点击事件处理
void MainWindow::trigerMenu(QAction *act)
{
    if (act->text() == "New Game")
    {
        bool mark;
        int size = QInputDialog::getInt(NULL, "Input a size",
                                        "Maximum Size: 12\n"
                                        "Generating a large puzzle may take seconds.\n"
                                        "A Size MUST be an EVEN number\t", 1, 1, 12, 1, &mark);
        while (size % 2 && mark)
        {
            size = QInputDialog::getInt(NULL, "Input a size",
                                        "Maximum Size: 12\n"
                                        "Generating a large puzzle may take seconds.\n"
                                        "Not an EVEN number, please input again\t", 1, 1, 12, 1, &mark);
        }
        if(mark)
        {
            board *b = GenBoard(size, 0.5);
            h = ConvertBoard(b);
            ClaNode *head = CopyClaSet(h);
            int *res = DPLL(head);
            int *puz = BoardToVector(b);
            InitGame(puz, res, size);
        }
    }
    if (act->text() == "Choose File...")
    {
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        tr("Choose CNF File"),
                                                        tr("<Enter File Name>"),
                                                        tr("*.cnf"));
        if(fileName.length())
        {
            QString fileSave = QFileDialog::getSaveFileName(this,
                                                            tr("Choose RES File"),
                                                            tr("new"),
                                                            tr("*.res"));
            if (fileSave.length())
            {
                FILE *DSR = fopen(fileName.toLatin1(), "r");
                FILE *DSW = fopen(fileSave.toLatin1(), "w");
                setbuf(DSW, NULL);
                fflush(DSW);
                ClaNode *read = CNF_Read(DSR);
                RES_Write(DSW, read, DPLL);
                QMessageBox::information(NULL, "Status", "Done.\t",
                                         QMessageBox::Yes, QMessageBox::Yes);
                fclose(DSR);
                fclose(DSW);
            }
        }
    }
    if (act->text() == "Rules")
    {
        QMessageBox::information(NULL, "Rules","You can only put \"1\" or \"0\" in a box.\n\n"
                                                  "Rule 1: No more than two similar numbers next to each other horizontally and vertically.\n"
                                                  "Rule 2: Each row and each column should contain an equal number of zeros and ones.\n"
                                                  "Rule 3: No same rows and same columns.",
                                 QMessageBox::Yes, QMessageBox::Yes);
    }
    if (act->text() == "Save as CNF")
    {
        if(!h)
        {
            QMessageBox::information(NULL, "Error", "Generate a puzzle FIRST!\t");
        }
        else
        {
            QString puzzleSave = QFileDialog::getSaveFileName(this,
                                                            tr("<Enter File Name>"),
                                                            tr("new"),
                                                            tr("*.cnf"));
            if(puzzleSave.length())
            {
                FILE *DSS = fopen(puzzleSave.toLatin1(), "w");
                setbuf(DSS, NULL);
                fflush(DSS);
                CNF_Save(h, DSS);
                QMessageBox::information(NULL, "Status", "Done.\t",
                                         QMessageBox::Yes, QMessageBox::Yes);
                fclose(DSS);
            }
        }
    }
    if (act->text() == "About")
    {
        QMessageBox::information(NULL, "HUST Program Design", "Binary Sudoku Game with DPLL\n\n"
                                                              "Author:\tShuhan Li\t\n"
                                                              "Class:\tCSEE1801\t\n"
                                                              "ID:\tU201814469\t\n",
                                 QMessageBox::Yes, QMessageBox::Yes);
    }
}
//游戏成功处理
void MainWindow::SucceedGame()
{
    for (int i = 0; i < data.length(); i++)
    {
        if (puzzle[i] != result[i])
            return;
    }
    for (int i = 0; i < data.length(); i++)
    {
        data[i]->setEnabled(false);
    }
    QMessageBox::information(NULL, "Congratulations!", "You Win!\t",
                             QMessageBox::Yes, QMessageBox::Yes);
}
MainWindow::~MainWindow()
{
}
