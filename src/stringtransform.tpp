template <StringTransform Trans>
struct TransformPolicy;

template <> struct TransformPolicy<StringTransform::RemoveSpaces> {
  static char transform(char c, char) {
    return std::isspace(c) ? 0 : c;
  }
};


template <> struct TransformPolicy<StringTransform::NormalizeSpaces> {
  static char transform(char c, char prev) {
    if (std::isspace(c) && std::isspace(prev)) return 0;
    if (std::isspace(c) && not std::isspace(prev)) return ' ';
    return c;
  }
};

template <> struct TransformPolicy<StringTransform::RemovePunctuation> {
  static char transform(char c, char) {
    return std::ispunct(c) ? 0 : c;
  }
};

template <> struct TransformPolicy<StringTransform::ToLower> {
  static char transform(char c, char) {
    return std::tolower(c);
  }
};

template <> struct TransformPolicy<StringTransform::ToUpper> {
  static char transform(char c, char) {
    return std::toupper(c);
  }
};
