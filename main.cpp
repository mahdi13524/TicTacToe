#include <QApplication>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVector>
#include <array>

class TicTacToeWindow : public QWidget {
public:
    TicTacToeWindow() {
        setWindowTitle(QStringLiteral("Tic Tac Toe | Qt C++"));
        setMinimumSize(520, 650);
        resize(560, 700);
        setStyleSheet(R"(
            QWidget { background: #101322; color: #f2f4ff; font-family: "Segoe UI"; }
            QLabel#title { color: #f5f6ff; font-size: 27px; font-weight: 800; }
            QLabel#subtitle { color: #8b91ad; font-size: 11px; }
            QLabel#status { color: #e6e9ff; background: #1b2035; border: 1px solid #303752; border-radius: 12px; padding: 13px; font-size: 14px; font-weight: 700; }
            QLabel#scoreCard { background: #1b2035; border: 1px solid #303752; border-radius: 12px; padding: 12px; color: #aeb5d3; font-size: 12px; }
            QPushButton#cell { background: #1a2034; border: 1px solid #303a57; border-radius: 15px; color: #f5f6ff; font-size: 54px; font-weight: 800; }
            QPushButton#cell:hover { background: #252e49; border: 1px solid #64739c; }
            QPushButton#reset { background: #8b7cff; color: #ffffff; border: none; border-radius: 11px; padding: 12px 20px; font-size: 12px; font-weight: 800; }
            QPushButton#reset:hover { background: #a69bff; }
            QPushButton#secondary { background: #252b42; color: #dce1ff; border: 1px solid #394361; border-radius: 11px; padding: 12px 20px; font-size: 12px; font-weight: 700; }
            QPushButton#secondary:hover { background: #303a58; }
        )");

        auto* root = new QVBoxLayout(this);
        root->setContentsMargins(28, 25, 28, 25);
        root->setSpacing(18);

        auto* heading = new QVBoxLayout;
        auto* title = new QLabel(QStringLiteral("TIC TAC TOE"));
        title->setObjectName("title");
        auto* subtitle = new QLabel(QStringLiteral("CLASSIC DUEL  •  TWO PLAYERS  •  DARK MODE"));
        subtitle->setObjectName("subtitle");
        heading->addWidget(title);
        heading->addWidget(subtitle);

        auto* scoreRow = new QHBoxLayout;
        scoreX_ = new QLabel;
        scoreO_ = new QLabel;
        scoreDraw_ = new QLabel;
        scoreX_->setObjectName("scoreCard");
        scoreO_->setObjectName("scoreCard");
        scoreDraw_->setObjectName("scoreCard");
        scoreX_->setAlignment(Qt::AlignCenter);
        scoreO_->setAlignment(Qt::AlignCenter);
        scoreDraw_->setAlignment(Qt::AlignCenter);
        scoreRow->addWidget(scoreX_);
        scoreRow->addWidget(scoreO_);
        scoreRow->addWidget(scoreDraw_);

        status_ = new QLabel;
        status_->setObjectName("status");
        status_->setAlignment(Qt::AlignCenter);

        auto* board = new QGridLayout;
        board->setSpacing(10);
        for (int i = 0; i < 9; ++i) {
            auto* button = new QPushButton;
            button->setObjectName("cell");
            button->setMinimumSize(130, 125);
            cells_[i] = button;
            board->addWidget(button, i / 3, i % 3);
            connect(button, &QPushButton::clicked, this, [this, i]() { play(i); });
        }

        auto* actions = new QHBoxLayout;
        auto* newRound = new QPushButton(QStringLiteral("دور جدید"));
        newRound->setObjectName("reset");
        auto* resetScores = new QPushButton(QStringLiteral("پاک‌کردن امتیازها"));
        resetScores->setObjectName("secondary");
        actions->addWidget(newRound);
        actions->addWidget(resetScores);

        connect(newRound, &QPushButton::clicked, this, [this]() { startRound(); });
        connect(resetScores, &QPushButton::clicked, this, [this]() {
            scoreXCount_ = 0;
            scoreOCount_ = 0;
            drawCount_ = 0;
            updateScores();
            startRound();
        });

        root->addLayout(heading);
        root->addLayout(scoreRow);
        root->addWidget(status_);
        root->addLayout(board, 1);
        root->addLayout(actions);

        updateScores();
        startRound();
    }

private:
    void play(int index) {
        if (finished_ || cellsState_[index] != ' ') return;

        cellsState_[index] = currentPlayer_;
        cells_[index]->setText(QString(QChar(currentPlayer_)));
        if (currentPlayer_ == 'X') {
            cells_[index]->setStyleSheet("QPushButton#cell { color: #78d9ff; background: #172c40; border: 1px solid #34799b; }");
        } else {
            cells_[index]->setStyleSheet("QPushButton#cell { color: #ff91c7; background: #352037; border: 1px solid #98547b; }");
        }

        const QVector<int> line = winningLine();
        if (!line.isEmpty()) {
            finished_ = true;
            if (currentPlayer_ == 'X') ++scoreXCount_;
            else ++scoreOCount_;
            for (int cell : line) {
                cells_[cell]->setStyleSheet("QPushButton#cell { color: #b7ff8a; background: #253d31; border: 2px solid #70bd7a; }");
            }
            status_->setText(QStringLiteral("بازیکن %1 برنده شد!").arg(QChar(currentPlayer_)));
            updateScores();
            return;
        }

        bool full = true;
        for (char value : cellsState_) {
            if (value == ' ') {
                full = false;
                break;
            }
        }

        if (full) {
            finished_ = true;
            ++drawCount_;
            status_->setText(QStringLiteral("بازی مساوی شد!"));
            updateScores();
            return;
        }

        currentPlayer_ = currentPlayer_ == 'X' ? 'O' : 'X';
        status_->setText(QStringLiteral("نوبت بازیکن %1").arg(QChar(currentPlayer_)));
    }

    QVector<int> winningLine() const {
        static const int lines[8][3] = {
            {0, 1, 2}, {3, 4, 5}, {6, 7, 8},
            {0, 3, 6}, {1, 4, 7}, {2, 5, 8},
            {0, 4, 8}, {2, 4, 6}
        };
        for (const auto& line : lines) {
            if (cellsState_[line[0]] != ' ' &&
                cellsState_[line[0]] == cellsState_[line[1]] &&
                cellsState_[line[1]] == cellsState_[line[2]]) {
                return {line[0], line[1], line[2]};
            }
        }
        return {};
    }

    void startRound() {
        cellsState_.fill(' ');
        currentPlayer_ = 'X';
        finished_ = false;
        for (auto* cell : cells_) {
            cell->setText(QString());
            cell->setStyleSheet(QString());
        }
        status_->setText(QStringLiteral("نوبت بازیکن X"));
    }

    void updateScores() {
        scoreX_->setText(QStringLiteral("بازیکن X\n%1").arg(scoreXCount_));
        scoreO_->setText(QStringLiteral("بازیکن O\n%1").arg(scoreOCount_));
        scoreDraw_->setText(QStringLiteral("مساوی\n%1").arg(drawCount_));
    }

    std::array<QPushButton*, 9> cells_{};
    std::array<char, 9> cellsState_{};
    QLabel* status_ = nullptr;
    QLabel* scoreX_ = nullptr;
    QLabel* scoreO_ = nullptr;
    QLabel* scoreDraw_ = nullptr;
    char currentPlayer_ = 'X';
    bool finished_ = false;
    int scoreXCount_ = 0;
    int scoreOCount_ = 0;
    int drawCount_ = 0;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");
    TicTacToeWindow window;
    window.show();
    return app.exec();
}
