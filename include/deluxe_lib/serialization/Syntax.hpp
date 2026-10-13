#ifndef SERIALIZATION_FORMAT_HPP
#define SERIALIZATION_FORMAT_HPP

#include <istream>
#include <ostream>
#include <string>

#include <nlohmann/json.hpp>

namespace serialization {

// Формат = способ рендерить/парсить nlohmann::json в поток.
class Format
{
public:
    virtual ~Format() = default;
    virtual const char* name() const noexcept = 0;
    virtual void write(const nlohmann::json& j, std::ostream& os) const = 0;
    virtual bool read (std::istream& is, nlohmann::json& j) const = 0;
};

// ---- JSON: pretty или compact ----
class JsonFormat : public Format
{
public:
    explicit JsonFormat(int indent = 2) : indent_(indent) {}

    const char* name() const noexcept override { return "json"; }

    void write(const nlohmann::json& j, std::ostream& os) const override
    {
        if (indent_ < 0) os << j.dump();
        else             os << j.dump(indent_);
    }

    bool read(std::istream& is, nlohmann::json& j) const override
    {
        try { is >> j; return !j.is_null() || is.good() || is.eof(); }
        catch (...) { return false; }
    }

private:
    int indent_;
};

// ---- key=value строки ----
class KeyValueFormat : public Format
{
public:
    const char* name() const noexcept override { return "keyvalue"; }

    void write(const nlohmann::json& j, std::ostream& os) const override
    {
        if (!j.is_object()) { os << j.dump() << '\n'; return; }
        for (auto it = j.begin(); it != j.end(); ++it) {
            os << it.key() << '=';
            if (it->is_string())
                os << escape(it->get<std::string>());
            else
                os << it->dump();
            os << '\n';
        }
    }

    bool read(std::istream& is, nlohmann::json& j) const override
    {
        j = nlohmann::json::object();
        std::string line;
        while (std::getline(is, line)) {
            if (line.empty()) continue;
            const auto eq = line.find('=');
            if (eq == std::string::npos) continue;

            std::string key = line.substr(0, eq);
            std::string raw = line.substr(eq + 1);

            // пробуем распарсить как скаляр JSON (число, true/false),
            // если не вышло — считаем строкой с unescape.
            try {
                j[key] = nlohmann::json::parse(raw);
            } catch (...) {
                j[key] = unescape(raw);
            }
        }
        return true;
    }

private:
    static std::string escape(const std::string& in)
    {
        std::string out;
        out.reserve(in.size());
        for (char c : in) {
            if      (c == '\\') out += "\\\\";
            else if (c == '\n') out += "\\n";
            else                out.push_back(c);
        }
        return out;
    }

    static std::string unescape(const std::string& in)
    {
        std::string out;
        out.reserve(in.size());
        for (size_t i = 0; i < in.size(); ++i) {
            if (in[i] == '\\' && i + 1 < in.size()) {
                ++i;
                if      (in[i] == 'n')  out.push_back('\n');
                else if (in[i] == '\\') out.push_back('\\');
                else                    out.push_back(in[i]);
            } else {
                out.push_back(in[i]);
            }
        }
        return out;
    }
};

} // namespace serialization

#endif // SERIALIZATION_FORMAT_HPP