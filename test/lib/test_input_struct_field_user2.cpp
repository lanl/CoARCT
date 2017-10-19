struct Y{
  int m_sfield1;
  float m_sfield2;
  Y() : m_sfield1(0), m_sfield2(1.0f){}
};

class X{
  int m_fld1;
  const char * m_fld2;
  Y m_fld3;
public:
  X() : m_fld2("lel"){}
  virtual ~X(){}

  void func1(const char *s){
    m_fld1 += 2;
    m_fld2 = s;
  }
  int func2(){
    return m_fld1 + 5;
  }
  void func3(Y&y){
    int & i = m_fld1;
    y.m_sfield2 += 1.2f + i++;
  }
  int func4(){
    func3(m_fld3);
    return m_fld3.m_sfield1;
  }
};
