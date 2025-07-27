template <StringTransform Trans>
struct TransformPolicy;

template <> struct TransformPolicy<StringTransform::RemoveSpaces> {
  static char transform(char c) {
    return std::isspace(c) ? 0 : c;
  }
};

template <> struct TransformPolicy<StringTransform::RemovePunctuation> {
  static char transform(char c) {
    return std::ispunct(c) ? 0 : c;
  }
};

template <> struct TransformPolicy<StringTransform::ToLower> {
  static char transform(char c) {
    return std::tolower(c);
  }
};

template <> struct TransformPolicy<StringTransform::ToUpper> {
  static char transform(char c) {
    return std::toupper(c);
  }
};
