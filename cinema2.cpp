#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <set>
#include <map>
#include <sstream>
#include <exception>
#include <memory>
#include <stack>
#include <algorithm>
using namespace std;


/*
 1) name="Zack"
 2) code=1987
 3) film="Star Wars"
 1-3 basic expressions, do not unfold further
 4) exp && exp      | name="Zack" && film="Star Wars"
 5) exp || exp      | name="Zack" || code=1987
 6) ! exp           | ! code=1987
 "!" > "&&" > "||"
 7) "(" / ")" are available
 Ex: name="Zack" && (! code=1984 || code=1987) || ! film="Star Wars"
        {basic} {op}({op}{basic}{op} {basic}) {op}{op}{basic}
     delete all directors, who named "Zack" AND whose code (1987 OR NOT 1984) OR
                directors, who didn't film "Star Wars"
удалили всех тех, чье имя Зак и чей код не 1984 или тех, кто не снимал Звездные войны
  */


void print_set(const set<int>& s){
    for (auto v : s)
        cout << v << " ";
    cout << endl;
}

void print_vector(const vector<string>& v){
    for (auto& item : v)
        cout << item << ", ";
    cout << endl;
}


struct BadCommand : std::exception {
private:
    std::string message;
public:
    explicit BadCommand(const std::string& command){
        message = "No such command: " + command;
    };
    [[nodiscard]] const char *what() const noexcept override {
        return message.c_str();
    }
};

struct BadData : std::exception {
private:
    std::string message;
public:
    //explicit BadCommand();
};


struct Actor {
    int unique_act_code;
    string FullName;
    int birth_year;
    std::vector<std::string> awards;
    std::set<std::string> films;
};

struct Film {
    std::string title;
    std::string studio_name;
    int realise_date; // только год - в исторической перспективе важен лишь он
    int unique_dir_code;
    std::set<int> unique_act_codes;
};

struct Director {
    Director() = default;;
    Director(int code, const string& name, const vector<string>& awards_,
             const set<string>& films_) {
        unique_dir_code = code;
        FullName = name;
        for (const auto& aw : awards_)
            awards.push_back(aw);
        films = films_;
    };

    int unique_dir_code{};
    string FullName; // Name & surname
    std::vector<std::string> awards; // set?
    std::set<std::string> films;

    void print() const {
        std::cout << "ID: " << unique_dir_code << '\n';
        std::cout << "Name & surname: " << FullName << endl;
        std::cout << "Awards: ";
        for (const auto& award : awards)
            std::cout << award << " ";
        std::cout << "\nFilms: ";
        for (const auto& film : films)
            std::cout << film << " ";
        std::cout << std::endl;
    }
};

bool operator <(const Director& d1, const Director& d2) {
    return d1.unique_dir_code < d2.unique_dir_code;
}


class DataBase {
private:
    std::map<int, Director> directors;
    std::map<int, Actor> actors;
    std::map<std::string, Film> films; // можно map<pair<string, int>> -> название | код режис
public:
    void add_director(const Director& dir);
    void delete_director(int unique_code);
    void delete_director(const string& expression);
    void add_actor(const Actor& actor);
    void delete_actor(int unique_code);
    void add_film(const Film& film);
    void delete_film(const std::string& title);
    void print_films(int unique_code);
    void print_films(const string& expression);
    [[nodiscard]] map<int, Director> give_directors() const{
        return directors;
    }
    [[nodiscard]] set<int> get_dir_by_film(const string& film) const;
    [[nodiscard]] set<int> get_filmDir_code_by_title(const string& title) const;
    [[nodiscard]] set<int> get_filmDir_code_by_DirName(const string& DirName) const;
    [[nodiscard]] set<int> get_DirCode_by_year(int year, const string& op);
};

/*class Expression;
vector<string> parse_tokens(const string& token);
template <typename Iterator>
shared_ptr<Expression> Parse(Iterator token, Iterator end);*/

void DataBase::add_director(const Director &dir) {
    if (directors.count(dir.unique_dir_code) == 0){
        directors[dir.unique_dir_code] = dir;
        return;
    }
    //throw BadData;
}

void DataBase::delete_director(int unique_code) {
    if (directors.count(unique_code) > 0) {
        std::set<std::string> all_dir_films = directors[unique_code].films;
        directors.erase(unique_code);
        for (const auto& film : all_dir_films){
            films.erase(film);
        }
    }
    // throw BadData;
}

void DataBase::add_actor(const Actor &actor) {
    if (actors.count(actor.unique_act_code) == 0){
        actors[actor.unique_act_code] = actor;
        return;
    }
    // throw BadData;
}

void DataBase::delete_actor(int unique_code) {
    if (actors.count(unique_code) > 0){
        std::set<std::string> all_act_films = actors[unique_code].films;
        actors.erase(unique_code);
        for (const auto& film : all_act_films)
            films.erase(film);
    }
    // throw BadData;
}

void DataBase::add_film(const Film &film) {
    if (films.count(film.title) == 0){
        films[film.title] = film;
        return;
    }
    // throw BadData;
}

void DataBase::delete_film(const std::string& title) {
    if (films.count(title) > 0){
        directors[films[title].unique_dir_code].films.erase(title);
        for (auto actor_code : films[title].unique_act_codes)
            actors[actor_code].films.erase(title);
    }
    // throw BadData;
}

void DataBase::print_films(int unique_code) {
    if (directors.count(unique_code) > 0){
        for (const auto& film : directors[unique_code].films){
            if (films.count(film) > 0) {
                std::string title = films[film].title;
                int release_year = films[film].realise_date;
                std::string studio = films[film].studio_name;
                std::cout << "Film title : " << title << ", release year : " << release_year
                          << ", film studio: " << studio << "\n";
            }
            // throw BadData;
        }
    }
    // throw BadData;
}

set<int> DataBase::get_dir_by_film(const string &film) const {
    set<int> result = {};
    for (const auto& [code, dir] : directors){
        if (dir.films.find(film) != dir.films.end()) { //т.е. найден режиссер этого фильма
            result.insert(code);
        }
    }
    return result;
}

set<int> DataBase::get_filmDir_code_by_title(const string &title) const {
    set<int> result = {};
    for (const auto& [title_, film] : films)
        if (title_ == title){
            result.insert(film.unique_dir_code);
        }
    return result;
}

set<int> DataBase::get_filmDir_code_by_DirName(const string &DirName) const {
    set<int> result = {};
    for (const auto& [code, dir] : directors)
        if (dir.FullName == DirName){
            result.insert(code);
        }
    return result;
}

set<int> DataBase::get_DirCode_by_year(int year, const string &op) {
    set<int> result;
    if (op == "==") {
        for (const auto& [title, film] : films)
            if (film.realise_date == year)
                result.insert(film.unique_dir_code);
    }
    else if (op == "!=") {
        for (const auto& [title, film] : films)
            if (film.realise_date != year)
                result.insert(film.unique_dir_code);
    }
    else if (op == "<=") {
        for (const auto& [title, film] : films)
            if (film.realise_date <= year)
                result.insert(film.unique_dir_code);
    }
    else if (op == "<") {
        for (const auto& [title, film] : films)
            if (film.realise_date < year)
                result.insert(film.unique_dir_code);
    }
    else if (op == ">") {
        for (const auto& [title, film] : films)
            if (film.realise_date > year)
                result.insert(film.unique_dir_code);
    }
    else if (op == ">=") {
        for (const auto& [title, film] : films)
            if (film.realise_date >= year)
                result.insert(film.unique_dir_code);
    }
    return result;
}

class Expression {
public:
    virtual set<int> Evaluate(DataBase& db) const = 0;
};

string without(const string& str){
    string without(next(str.begin()), prev(str.end()));
    return without;
}

class Basic : public Expression {
public:
    explicit Basic(string  command) : _command(std::move(command)) {}

    set<int> Evaluate(DataBase& db) const override {
        set<int> result = {};
        vector <string> words, Rwords;

        stringstream stringStream(_command);
        stringstream RstringStream(_command);
        string word;

        while (getline(stringStream, word, '='))
            words.push_back(word);

        while (getline(RstringStream, word, ' '))
            Rwords.push_back(word);

        //print_vector(words);
        //print_vector(Rwords);

        if (words[0] == "name") {
            auto dir_map = db.give_directors();
            for (const auto& [code, dir] : dir_map){
                if (dir.FullName == without(words[1])){
                    result.insert(code);
                    break;
                }
            }
        }
        else if (words[0] == "code") {
            int code = atoi(words[1].c_str());
            if (db.give_directors().count(code) > 0) // есть режиссер с таким кодом
                result.insert(code);
        }
        else if (words[0] == "film")
            result = db.get_dir_by_film(without(words[1]));
        else if (words[0] == "title")
            result = db.get_filmDir_code_by_title(without(words[1]));
        else if (words[0] == "director") {
            result = db.get_filmDir_code_by_DirName(without(words[1]));
            //cout << "DirName result: " << endl;
            //print_set(result);
        }
        else if (Rwords[0] == "year")
            result = db.get_DirCode_by_year(atoi(Rwords[2].c_str()), Rwords[1]);

        return result;
    }

private:
    string _command;
};

class Op : public Expression {
public:
    explicit Op(const string& value)
            : precedence([value] {
        if (value == "(") {
            return 0;
        }
        else if (value == "!") {
            return 3;
        }
        else if (value == "&&") {
            return 2;
        }
        else if (value == "||") {
            return 1;
        }
    }()),
              _op(value) {}
    const uint8_t precedence;

    set<int> Evaluate(DataBase& db) const override {
        set<int> result;

        if (_op == "!") {
            set<int> right_value = _right->Evaluate(db);
            set<int> keys;
            for (const auto& [key, Dir] : db.give_directors())
                keys.insert(key);

            //cout << "sets before set_difference: \n";
            //print_set(right_value);

            set_difference(begin(keys), end(keys), begin(right_value),
                           end(right_value), inserter(result, result.end()));

            //cout << "Set after set_difference\n";
            //print_set(result);
        }
        else if (_op == "&&") {
            auto left = _left->Evaluate(db);
            auto right = _right->Evaluate(db);

            //cout << "sets before intersection: \n";
            //print_set(left);
            //print_set(right);

            set_intersection(begin(left), end(left), begin(right), end(right),
                             inserter(result, result.end()));

            //cout << "Set after intersection:\n";
            //print_set(result);
        }
        else if (_op == "||") {
            auto left = _left->Evaluate(db);
            auto right = _right->Evaluate(db);

            //cout << "sets before union: \n";
            //print_set(left);
            //print_set(right);

            set_union(begin(left), end(left),begin(right), end(right),
                      inserter(result, result.end()));

            //cout << "set after union:\n";
            //print_set(result);
        }
        return result;
    }

    [[nodiscard]] string Get_op() const {
        return _op;
    }

    void SetLeft(const shared_ptr<Expression>& exp) { _left = exp; }
    void SetRight(const shared_ptr<Expression>& exp) { _right = exp; }

private:
    const string _op;
    shared_ptr<const Expression> _left, _right;
};

template <typename Iterator>
shared_ptr<Expression> Parse(Iterator token, Iterator end) {
    if (token == end) {
        return make_shared<Basic>("0");
    }

    stack<shared_ptr<Expression>> values;
    stack<shared_ptr<Op>> ops;

    auto PopOps = [&](int precedence) {
        while (!ops.empty() && ops.top()->precedence >= precedence) {

            if (ops.top()->Get_op() == "(") {
                ops.pop();
                continue;
            }
            else if (ops.top()->Get_op() == "!") {
                auto value1 = values.top();
                values.pop();
                auto op = ops.top();
                ops.pop();
                op->SetRight(value1);
                values.push(op);
                continue;
            }

            auto value1 = values.top();
            values.pop();
            auto value2 = values.top();
            values.pop();
            auto op = ops.top();
            ops.pop();
            op->SetRight(value1);
            op->SetLeft(value2);
            values.push(op);
        }
    };

    while (token != end) {
        auto value = *token;

        if (value == "(") {
            ops.push(make_shared<Op>(value));
        }
        else if (value == ")") {
            PopOps(0);
        }
        else if (value == "!") {
            PopOps(3);
            ops.push(make_shared<Op>(value));
        }
        else if (value == "&&") {
            PopOps(2);
            ops.push(make_shared<Op>(value));
        }
        else if (value == "||") {
            PopOps(1);
            ops.push(make_shared<Op>(value));
        }
        else {
            values.push(make_shared<Basic>(value));
        }

        ++token;
    }

    while (!ops.empty()) {
        PopOps(0);
    }

    return values.top();
}


vector<string> parse_tokens(const string& token) {
    istringstream iss(token, istringstream::in);

    vector<string> tokens;
    string word;

    while (iss >> word) {
        //cout << word << "* ";
        tokens.push_back(word);
    }

    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i].compare(0, 4, "year") == 0) {
            string year_command = tokens[i] + " " + tokens[i + 1] + " " + tokens[i + 2];
            tokens.erase(tokens.begin() + i, tokens.begin() + i + 3);
            tokens.insert(tokens.begin() + i, year_command);
        }
        else if (tokens[i].compare(0, 4, "name") == 0) {
            string name_command;
            if (*prev(tokens[i].end()) == '"'){
                name_command = tokens[i];
                tokens.erase(tokens.begin() + i, tokens.begin() + i + 1);
            }
            else {
                name_command = tokens[i] + " " + tokens[i + 1];
                tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            }
            tokens.insert(tokens.begin() + i, name_command);
        }
        else if (tokens[i].compare(0, 8, "director") == 0) {
            string dir_command;
            if (*prev(tokens[i].end()) == '"'){
                dir_command = tokens[i];
                tokens.erase(tokens.begin() + i, tokens.begin() + i + 1);
            }
            else {
                dir_command = tokens[i] + " " + tokens[i + 1];
                tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            }
            tokens.insert(tokens.begin() + i, dir_command);
        }
        else if (tokens[i].compare(0, 4, "film") == 0) {
            string film_command;
            if (*prev(tokens[i].end()) == '"') { // т.е. фильм из 1 слова
                film_command=tokens[i];
                tokens.erase(tokens.begin() + i, tokens.begin() + i + 1);
            }
            else {
                film_command = tokens[i] + " " + tokens[i + 1];
                tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            }
            tokens.insert(tokens.begin() + i, film_command);
        }
        else if (tokens[i].compare(0, 5, "title") == 0) {
            string title_command;
            if (*prev(tokens[i].end()) == '"'){
                title_command = tokens[i];
                tokens.erase(tokens.begin() + i, tokens.begin() + i + 1);
            }
            else {
                title_command = tokens[i] + " " + tokens[i + 1];
                tokens.erase(tokens.begin() + i, tokens.begin() + i + 2);
            }
            tokens.insert(tokens.begin() + i, title_command);
        }
    }

    return tokens;
}

void DataBase::delete_director(const string& expression) {
    vector<string> test = parse_tokens(expression);
    auto result_SharedPTR = Parse(test.begin(), test.end());
    set<int> result = result_SharedPTR->Evaluate(*this);
    for (const auto& i : result){
        delete_director(i);
    }
}

void DataBase::print_films(const string &expression) {
    vector<string> test = parse_tokens(expression);
    auto result_SharedPTR = Parse(test.begin(), test.end());
    set<int> result = result_SharedPTR->Evaluate(*this);
    for (const auto& i : result){
        print_films(i);
    }
}

// returns request_code
int operation_validator(std::string& req_line){
    std::stringstream ss;
    ss << req_line;
    std::string command, subcommand;
    ss >> command;
    if (command == "quit")
        return 1;
    else if (command == "add"){
        ss >> subcommand;
        if (subcommand == "director")
            return 2;
        else if (subcommand == "actor")
            return 4;
        else if (subcommand == "film")
            return 6;
        else
            throw BadCommand(command + " " + subcommand);
    }
    else if (command == "delete"){
        ss >> subcommand;
        if (subcommand == "director")
            return 3;
        else if (subcommand == "actor")
            return 5;
        else if (subcommand == "film")
            return 7;
        else
            throw BadCommand(command + " " + subcommand);
    }
    else if (command == "print"){
        ss >> subcommand;
        if (subcommand == "films")
            return 8;
        throw BadCommand(command + " " + subcommand);
    }
    throw BadCommand(command);
}

bool solver(const std::string& full_line, int com_id, DataBase& db){
    std::stringstream ss;
    ss << full_line;
    int unique_id, year;
    std::string to_skip, name, surname, text; // text for awards / film_title
    switch (com_id) {
        case 1:
            return true;
            break;
        case 2:
            std::cout << "You here2\n";
            std::vector<std::string> awards;
            std::set<std::string> films;
            ss >> to_skip;
            ss >> to_skip;
            ss >> unique_id;
            ss.ignore(2) ; //space"
            ss >> name;
            ss.ignore(1); // ,
            ss >> surname;
            ss.ignore(2);
            std::cout << "lvl 1: " << ss.peek() << '\n';
            while (true) {
                ss >> text;
                std::cout << text << '\n';
                awards.push_back(text.substr(0,text.size() - 1));
                if (text[text.size() - 1] != '"')
                    ss.ignore(1);
                else
                    break;
            }
            ss.ignore(2);
            //std::cout << "lvl 2\n";
            while (!ss.eof()) {
                ss >> text;
                films.insert(text.substr(0,text.size() - 1));
                ss.ignore(1);
            }
            string FullName = name + " " + surname;
            //add director 1 Nik Kik aw1, aw2, aw3 film1, film2, film3
            Director dir = {
                    unique_id,
                    FullName,
                    awards,
                    films};
            dir.print();
            db.add_director(dir);
            return false;
    }
}


map<int, vector<string>> dir_film_titles(const DataBase& db){
    auto almost = db.give_directors();
    map<int, vector<string>> result = {};
    for (const auto& [dir_code, dir] : almost){
        for(const auto& film : dir.films)
            result[dir_code].push_back(film);
    }
    return result;
}

void print_dir_films_map(const map<int, vector<string>>& dir_films){
    for (const auto& [dir_code, films] : dir_films) {
        cout << "Director's code:" << dir_code << " and his films: ";
        print_vector(films);
    }
}

void make_bd(DataBase& db){
    db.add_director({1, "Nikita Surkov",
                     {"My life", "ASAP"},
                     {"Golden Apple", "Winner"}});
    db.add_director({2, "Kira Yoshikage",
                     {"Best person"},
                     {"Jojo"}});
    db.add_director({3, "Ri Ru",
                     {"Best architect",},
                     {"My love"}});
    db.add_director({6, "Math Warrior",
                     {"Good_math teacher",},
                     {"Super_hard task"}});

    db.add_film({"Golden Apple", "Universal",
                 2001, 1, {}});
    db.add_film({"Winner", "Universal",
                 2021, 1, {}});
    db.add_film({"Jojo", "UniversalJap", 2018,
                 2, {}});
    db.add_film({"My love", "Universal",
                 2016, 3, {}});
    db.add_film({"Super_hard task", "Universal",
                 2019, 6, {}});
    auto before_tests = dir_film_titles(db);
    cout << "Director's codes and film's titles before tests:\n" << endl;
    print_dir_films_map(before_tests);

    //some tests here:
    db.delete_director(R"(film="Jojo" && ! code=1)");
    db.print_films(R"(( title="My love" || title="My dare" ) && ! director="Nikita Surkov" && ( year >= 2015 && year <= 2017 ))");

    cout << endl << "Director's codes and film's titles after tests:\n" << endl;
    auto after_tests = dir_film_titles(db);
    print_dir_films_map(after_tests);

}

/* TESTS:
const string expression = R"(film="Jojo" && ! ( code=10 || code=3 ) || name="Nikita Surkov")";
const string expression = R"(title="My love")";
const string expression = R"(! film="Jojo" && ! code=1)";
const string expression = R"(year > 2016)";
const string expression = R"(! film="Borya" && ( ! code=10 && ( code=1 || code=3 ) ) && ! name="Nikita Surkov")";
const string expression = R"(( title="My love" || title="My dare" ) && ! director="Nikita Surkov" && ( year >= 2015 && year <= 2017 ))";
  */

int main() {
    std::string command_line;
    int command_id;
    DataBase db;

    /* код из 1ой контрольной, TODO

    while(std::getline(std::cin, command_line)) {
        try {
            command_id = operation_validator(command_line);
            if (solver(command_line, command_id, db))
                break;
        } catch (BadCommand &bc) {
            std::cout << bc.what() << std::endl;
        }
    }*/


    // const string expression = R"()"; - можно написать в скобках свое выражение для теста
    /*
    auto test = parse_tokens(expression);
    auto result_SharedPTR = Parse(test.begin(), test.end());
    auto result = result_SharedPTR->Evaluate(db);
    result хранит множество уникальных кодов режиссеров, удовлетворяющих выражению
    */
    make_bd(db); // моя тестовая база данных
    return 0;
}