/*
 * Copyright (C) 2006-2013  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Gaia
 *
 * Gaia is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

#ifndef GAIA_PARSER_PARSERTYPES_H
#define GAIA_PARSER_PARSERTYPES_H

#include <QList>
#include "filter.h"
#include "grammar.h"

namespace gaia2 {
    
    template <typename SearchPointType, typename DataSetType>
    class BaseQueryOptimizer;
    
    class Filter;
    
    namespace parser {
        
        
        //////////////////////////////////////////////////////////////////////////////
        // ----[ Base types ]-------------------------------------------------------//
        
        class Predicate {
        public:
            virtual ~Predicate() {}
            virtual bool value() const = 0;
            inline bool isTrue() const { return value(); }
            virtual QString toString() const = 0;
            
            virtual Predicate* copy() const = 0;
        };
        
        class Value {
        public:
            virtual ~Value() {}
            virtual Real value() const = 0;
            virtual QString toString() const = 0;
            
            virtual Value* copy() const = 0;
        };
        
        class Label {
        public:
            virtual ~Label() {}
            virtual QString value() const = 0;
            virtual QString toString() const = 0;
            
            virtual Label* copy() const = 0;
        };
        
        
        
        //////////////////////////////////////////////////////////////////////////////
        // ----[ Predicate types ]--------------------------------------------------//
        
        class PredValueComparison : public Predicate {
            // note: access here should be protected, but since the SearchSpacePool has
            //       been templated, we can't forward-declare here it anymore
        public:
            Value *_lhs, *_rhs;
            int _type;
            friend class gaia2::Filter;
            
        public:
            PredValueComparison(Value* lhs, int type, Value* rhs)
            : _lhs(lhs), _rhs(rhs), _type(type) {}
            
            PredValueComparison(const PredValueComparison& other)
            : Predicate(), _lhs(other._lhs->copy()), _rhs(other._rhs->copy()), _type(other._type) {}
            
            ~PredValueComparison() {
                delete _lhs;
                delete _rhs;
            }
            
            virtual Predicate* copy() const {
                return new PredValueComparison(*this);
            }
            
            virtual bool value() const {
                switch (_type) {
                    case EQ:  return _lhs->value() == _rhs->value();
                    case NEQ: return _lhs->value() != _rhs->value();
                    case LT:  return _lhs->value() <  _rhs->value();
                    case GT:  return _lhs->value() >  _rhs->value();
                    case LTE: return _lhs->value() <= _rhs->value();
                    case GTE: return _lhs->value() >= _rhs->value();
                    default:
                        throw gaia2::GaiaException("Invalid type in PredValueComparison");
                }
                // should never get here
                return false;
            }
            
            
            virtual QString toString() const {
                QString result;
                switch (_type) {
                    case EQ:  result = "%1 == %2"; break;
                    case NEQ: result = "%1 != %2"; break;
                    case LT:  result = "%1 < %2";  break;
                    case GT:  result = "%1 > %2";  break;
                    case LTE: result = "%1 <= %2"; break;
                    case GTE: result = "%1 >= %2"; break;
                }
                
                return result.arg(_lhs->toString()).arg(_rhs->toString());
            }
        };
        
        class PredValueRange : public Predicate {
            // note: access here should be protected, but since the SearchSpacePool has
            //       been templated, we can't forward-declare here it anymore
        public:
            Value* _var;
            Real _min, _max;
            friend class gaia2::Filter;
            
        public:
            PredValueRange(Value* var, Real minv, Real maxv)
            : _var(var), _min(minv), _max(maxv) {
                if (minv > maxv) {
                    _min = maxv;
                    _max = minv;
                }
            }
            
            PredValueRange(const PredValueRange& other)
            : Predicate(), _var(other._var->copy()), _min(other._min), _max(other._max) {}
            
            ~PredValueRange() { delete _var; }
            
            virtual Predicate* copy() const {
                return new PredValueRange(*this);
            }
            
            virtual bool value() const {
                Real v = _var->value();
                return (v >= _min) && (v <= _max);
            }
            
            virtual QString toString() const {
                return QString("%1 BETWEEN %2 AND %3").arg(_var->toString()).arg(_min).arg(_max);
            }
        };
        
        class PredLabelComparison : public Predicate {
            // note: access here should be protected, but since the SearchSpacePool has
            //       been templated, we can't forward-declare here it anymore
        public:
            Label *_lhs, *_rhs;
            int _type;
            friend class gaia2::Filter;
            
        public:
            PredLabelComparison(Label* lhs, int type, Label* rhs)
            : _lhs(lhs), _rhs(rhs), _type(type) {}
            
            PredLabelComparison(const PredLabelComparison& other)
            : Predicate(), _lhs(other._lhs->copy()), _rhs(other._rhs->copy()), _type(other._type) {}
            
            ~PredLabelComparison() {
                delete _lhs;
                delete _rhs;
            }
            
            virtual Predicate* copy() const {
                return new PredLabelComparison(*this);
            }
            
            virtual bool value() const {
                switch (_type) {
                    case EQ: return _lhs->value() == _rhs->value();
                    case NEQ: return _lhs->value() != _rhs->value();
                    default:
                        throw gaia2::GaiaException("Invalid type in PredLabelComparison");
                }
                // should never get here
                return false;
            }
            
            virtual QString toString() const {
                QString result;
                switch (_type) {
                    case EQ:  result = "%1 == %2"; break;
                    case NEQ: result = "%1 != %2"; break;
                }
                
                return result.arg(_lhs->toString()).arg(_rhs->toString());
            }
        };
        
        class PredLogicalOp : public Predicate {
        protected:
            Predicate *_lhs, *_rhs;
            int _op;
            template <typename SearchPointType, typename DataSetType>
            friend class gaia2::BaseQueryOptimizer;
            friend class gaia2::Filter;
            
        public:
            PredLogicalOp(Predicate* lhs, int op, Predicate* rhs)
            : _lhs(lhs), _rhs(rhs), _op(op) {}
            
            PredLogicalOp(const PredLogicalOp& other)
            : Predicate(), _lhs(other._lhs->copy()), _rhs(other._rhs->copy()), _op(other._op) {}
            
            ~PredLogicalOp() {
                delete _lhs;
                delete _rhs;
            }
            
            virtual Predicate* copy() const {
                return new PredLogicalOp(*this);
            }
            
            virtual bool value() const {
                switch (_op) {
                    case AND: return _lhs->value() && _rhs->value();
                    case OR:  return _lhs->value() || _rhs->value();
                    default:
                        throw gaia2::GaiaException("Invalid type in PredLogicalOp");
                }
                // should never get here
                return false;
            }
            
            virtual QString toString() const {
                QString result;
                switch (_op) {
                    case AND: result = "(%1) AND (%2)"; break;
                    case OR:  result = "(%1) OR (%2)"; break;
                }
                
                return result.arg(_lhs->toString()).arg(_rhs->toString());
            }
        };
        
        class PredNegate : public Predicate {
        protected:
            Predicate* _pred;
            template <typename SearchPointType, typename DataSetType>
            friend class gaia2::BaseQueryOptimizer;
            friend class gaia2::Filter;
            
        public:
            PredNegate(Predicate* pred) : _pred(pred) {}
            PredNegate(const PredNegate& other) : Predicate(), _pred(other._pred->copy()) {}
            ~PredNegate() { delete _pred; }
            
            virtual Predicate* copy() const {
                return new PredNegate(*this);
            }
            
            virtual bool value() const {
                return !_pred->value();
            }
            
            virtual QString toString() const {
                return QString("NOT (%1)").arg(_pred->toString());
            }
        };
        
        class BooleanConstant : public Predicate {
        protected:
            bool _value;
            
        public:
            BooleanConstant(bool value) : _value(value) {}
            virtual Predicate* copy() const { return new BooleanConstant(*this); }
            
            inline virtual bool value() const { return _value; }
            
            virtual QString toString() const {
                return _value ? "True" : "False";
            }
        };
        
        class PredValueIsIn : public Predicate {
        protected:
            Value* _var;
            QList<Real> _vlist;
            friend class gaia2::Filter;
            
        public:
            PredValueIsIn(Value* var, const QList<Real>& vlist)
            : _var(var), _vlist(vlist) {}
            
            PredValueIsIn(const PredValueIsIn& other)
            : Predicate(), _var(other._var->copy()), _vlist(other._vlist) {}
            
            ~PredValueIsIn() {
                delete _var;
            }
            
            virtual Predicate* copy() const {
                return new PredValueIsIn(*this);
            }
            
            virtual bool value() const {
                return _vlist.contains(_var->value());
            }
            
            virtual QString toString() const {
                QString result = QString("%1 IN (").arg(_var->toString());
                if (!_vlist.empty()) result += QString::number(_vlist[0]);
                for (int i=1; i<_vlist.size(); i++) result += QString(", %1").arg(_vlist[i]);
                return result + ")";
            }
        };
        
        class PredLabelIsIn : public Predicate {
        protected:
            Label* _var;
            QStringList _slist;
            template <typename SearchPointType, typename DataSetType>
            friend class gaia2::BaseQueryOptimizer;
            friend class gaia2::Filter;
            
        public:
            PredLabelIsIn(Label* var, const QStringList& slist)
            : _var(var), _slist(slist) {}
            
            PredLabelIsIn(const PredLabelIsIn& other)
            : Predicate(), _var(other._var->copy()), _slist(other._slist) {}
            
            ~PredLabelIsIn() {
                delete _var;
            }
            
            virtual Predicate* copy() const {
                return new PredLabelIsIn(*this);
            }
            
            virtual bool value() const {
                return _slist.contains(_var->value());
            }
            
            virtual QString toString() const {
                QString result = QString("%1 IN (").arg(_var->toString());
                if (!_slist.empty()) result += QString("\"%1\"").arg(_slist[0]);
                for (int i=1; i<_slist.size(); i++) result += QString(", \"%1\"").arg(_slist[i]);
                return result + ")";
            }
        };
        
        
        //////////////////////////////////////////////////////////////////////////////
        // ----[ Label types ]------------------------------------------------------//
        
        class LabelConstant : public Label {
        protected:
            QString _value;
            
        public:
            LabelConstant(const QString& value) : _value(value) {}
            virtual Label* copy() const { return new LabelConstant(*this); }
            inline virtual QString value() const { return _value; }
            virtual QString toString() const { return QString("%1").arg(_value); }
        };
        
        class LabelVariable : public Label {
        protected:
            const gaia2::Point** _point;
            DescriptorType _type;
            DescriptorLengthType _ltype;
            int _idx;
            QString _varname; // name used by the user in the filter query
            QString _fullname; // fully qualified name, used for getting values when label is of EnumType
            
        public:
            LabelVariable(const QString& name) : _point(0), _ltype(FixedLength),
            _idx(-1), _varname(name) {}
            
            QString name() const { return _varname; }
            
            virtual Label* copy() const { return new LabelVariable(*this); }
            
            virtual QString value() const {
                if (_idx == -1) throw GaiaException("variable ", _varname, " is not bound...");
                if (!_point) throw GaiaException("no currently set point");
                
                switch (_type) {
                    case StringType:
                        switch (_ltype) {
                            case FixedLength:    return (*_point)->fstringData()[_idx];
                            case VariableLength: return (*_point)->vstringData()[_idx].toSingleValue();
                        }
                        throw GaiaException("Internal error at LabelVariable::value()");
                    case EnumType:
                        switch (_ltype) {
                            case FixedLength:    return (*_point)->layout().enumToString(_fullname, (*_point)->fenumData()[_idx]);
                            case VariableLength: return (*_point)->layout().enumToString(_fullname, (*_point)->venumData()[_idx].toSingleValue());
                        }
                        // no need to have a throw here, as we'll fall back on the outer one anyway
                    default:
                        ;
                }
                throw GaiaException("Internal error at LabelVariable::value()");
            }
            
            void bind(const PointLayout& layout, const Point** pointLocation) {
                _point = pointLocation;
                _fullname = layout.descriptorNames(layout.descriptorLocation(_varname))[0];
                
                Region location = layout.descriptorLocation(name());
                _type = location.type();
                _ltype = location.lengthType();
                _idx = location.index();
            }
            
            virtual QString toString() const { return _varname; }
        };
        
        class LabelPointID : public Label {
        protected:
            const Point** _point;
            
        public:
            LabelPointID() : _point(0) {}
            
            virtual Label* copy() const { return new LabelPointID(*this); }
            
            virtual QString value() const {
                if (!_point) throw GaiaException("no currently set point");
                return (*_point)->name();
            }
            
            void bind(const Point** pointLocation) {
                _point = pointLocation;
            }
            
            virtual QString toString() const { return "point.id"; }
        };
        
        
        //////////////////////////////////////////////////////////////////////////////
        // ----[ Value types ]------------------------------------------------------//
        
        class ValueConstant : public Value {
        protected:
            Real _value;
            
        public:
            ValueConstant(Real value) : _value(value) {}
            virtual Value* copy() const { return new ValueConstant(*this); }
            virtual Real value() const { return _value; }
            virtual QString toString() const { return QString::number(_value); }
        };
        
        class ValueVariable : public Value {
        protected:
            const gaia2::Point** _point;
            DescriptorLengthType _ltype;
            int _idx;
            QString _varname;
            int _dimension_number;
            
        public:
            
            ValueVariable(const QString& name) {
                _point = 0;
                _ltype = FixedLength;
                _idx = -1;
                _dimension_number = -1;

                /* check if name contains dimension number in the form n.a.m.e.[#], where # is the number.
                 * in case it contains the dimension number strip it from the name and save it to _dimension_number */

                if (name.lastIndexOf("[") != -1 && name.lastIndexOf("]") != -1){
                    _varname = name.left(name.lastIndexOf("["));

                    QString dimension_number_str = name.section('[', 1);
                    dimension_number_str.chop(1);
                    bool ok;
                    int dimension_number = dimension_number_str.toInt(&ok, 10);
                    if (ok) {
                        // conversion succeeded
                        _dimension_number = dimension_number;
                    }else{
                        throw GaiaException("dimension number malformated for descriptor ", _varname, "...");
                    }
                }else{
                    _varname = name;
                }
            }
            
            QString name() const {
                return _varname;
            }
            
            virtual Value* copy() const { return new ValueVariable(*this); }
            
            virtual Real value() const {
                if (_idx == -1) throw GaiaException("variable ", _varname, " is not bound...");
                if (!_point) throw GaiaException("no currently set point");
                
                switch (_ltype) {
                    case FixedLength: return (*_point)->frealData()[_idx];
                    case VariableLength: return (*_point)->vrealData()[_idx].toSingleValue();
                }
                throw GaiaException("Internal error at ValueVariable::value()");
            }
            
            void bind(const PointLayout& layout, const Point** pointLocation) {
                _point = pointLocation;
                _ltype = layout.descriptorLocation(name()).lengthType();
                
                // Detect if the descriptor is multidimensional by checking the length of listIndices (NOTE: requires fix length transformation)
                QVector< int > listIndices = layout.descriptorLocation(name()).listIndices(layout.descriptorLocation(name()).type(), layout.descriptorLocation(name()).lengthType());                
                if (listIndices.size() == 1){
                    // If descriptor is not multidimensional, we get the first index
                    _idx = layout.descriptorLocation(name()).index();
                }else{
                    // If descriptor is multidimensional, get the value indicated by _dimension_number
                    if (_dimension_number < 0){
                        throw GaiaException("dimension number was not indicated for multidimensional descriptor ", _varname, "...");
                    }else{
                        if (_dimension_number < listIndices.size()){
                            _idx = listIndices[_dimension_number];
                        }else{
                            throw GaiaException("dimension number out of range for multidimensional descriptor ", _varname, "...");
                        }
                    }
                }
            }
            
            virtual QString toString() const { return _varname; }
        };
    
    } // namespace parser
} // namespace gaia2


// useful define to trick lemon to generate code inside our namespace
#define OPEN_GAIA_PARSER_NS namespace gaia2 { namespace parser {
#define CLOSE_GAIA_PARSER_NS } } // namespace parser / namespace gaia2

#endif // GAIA_PARSER_PARSERTYPES_H
