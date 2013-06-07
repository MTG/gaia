#include <yaml.h>
#include <QFile>
#include <iostream>
#include "yamlcpp.h"

#ifdef USE_BZ2_ENCRYPTION
#include "bzlib.h"
#endif

using namespace std;

namespace gaia2 {
namespace yaml {

// adapted from the one in libyaml/tests/example-reformatter.c
QString errorMessage(const yaml_parser_t& parser);


#ifdef USE_BZ2_ENCRYPTION

Node loadFromByteArray(QByteArray& encrypted, bool markQuotedScalars) {
  // add some magic
  encrypted.prepend("BZ");

  char* buffer = 0;
  uint size = 4096;
  int status;
  do {
    delete buffer;
    size *= 2;
    buffer = new char[size];

    status = BZ2_bzBuffToBuffDecompress(buffer, &size,
                                        encrypted.data(), encrypted.size(),
                                        0, 0);
  } while (status == BZ_OUTBUFF_FULL);

  if (status != BZ_OK) {
    throw GaiaException("Error while trying to read encrypted file, error code: ", status);
  }

  Node result = load(buffer, size, markQuotedScalars);
  delete buffer;
  return result;
}

Node loadFromString(const string& str, bool markQuotedScalars) {
  QByteArray encrypted = QByteArray(str.c_str(), str.size());
  return loadFromByteArray(encrypted, markQuotedScalars);
}

Node loadFromFile(const QString& filename, bool markQuotedScalars) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    file.setFileName(QString(filename).remove("\\"));
    if (!file.open(QIODevice::ReadOnly)) {
      throw GaiaException("Yaml::loadFromFile: Could not open file: ", filename);
    }
  }

  QByteArray encrypted = file.readAll();
  return loadFromByteArray(encrypted, markQuotedScalars);
}

Node loadFromPlainFile(const QString& filename, bool markQuotedScalars) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    file.setFileName(QString(filename).remove("\\"));
    if (!file.open(QIODevice::ReadOnly)) {
      throw GaiaException("Yaml::loadFromFile: Could not open file: ", filename);
    }
  }

  QTextStream stream(&file);
  stream.setCodec("UTF-8");

  QByteArray asciiStr = stream.readAll().toUtf8();
  return yaml::load(asciiStr.constData(), asciiStr.size(), markQuotedScalars);
}

#else // USE_BZ2_ENCRYPTION

Node loadFromString(const string& str, bool markQuotedScalars) {
  return load(str.c_str(), (uint)str.size(), markQuotedScalars);
}

Node loadFromFile(const QString& filename, bool markQuotedScalars) {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    file.setFileName(QString(filename).remove("\\"));
    if (!file.open(QIODevice::ReadOnly)) {
      throw GaiaException("Yaml::loadFromFile: Could not open file: ", filename);
    }
  }

  QTextStream stream(&file);
  stream.setCodec("UTF-8");

  QByteArray asciiStr = stream.readAll().toUtf8();
  return yaml::load(asciiStr.constData(), asciiStr.size(), markQuotedScalars);
}

#endif // USE_BZ2_ENCRYPTION


Node load(const QString& str) {
  QByteArray s = str.toUtf8();
  return load(s.constData(), s.size());
}

Node load(const char* str, uint size, bool markQuotedScalars) {

  QList<Node> nodeStack;
  QList<int> mapPos;
  QList<int> seqPos;

  yaml_parser_t parser;
  yaml_event_t event;

  // initialize parser
  yaml_parser_initialize(&parser);

  // set input
  yaml_parser_set_input_string(&parser, (unsigned char*)str, size);

  bool done = false;

  do {

    if (!yaml_parser_parse(&parser, &event)) {
      throw YamlException(errorMessage(parser));
    }

    switch (event.type) {

    case YAML_STREAM_END_EVENT: done = true; break;

    case YAML_SCALAR_EVENT: {
      QString data = QString::fromUtf8((const char*)event.data.scalar.value, (int)event.data.scalar.length);
      // if scalar was quoted, mark it so that we do not try to convert it
      // automatically to a float
      if (markQuotedScalars &&
          (event.data.scalar.style == YAML_SINGLE_QUOTED_SCALAR_STYLE ||
           event.data.scalar.style == YAML_DOUBLE_QUOTED_SCALAR_STYLE)) {
        data = QChar::Null + data;
      }
      nodeStack.push_back(Node(data));

      break;
    }

    case YAML_SEQUENCE_START_EVENT:
      seqPos.push_back(nodeStack.size());
      break;

    case YAML_SEQUENCE_END_EVENT: {
      // create new sequence
      Sequence seq;
      Sequence::const_iterator it = nodeStack.constBegin() + seqPos.back();
      for (; it != nodeStack.end(); ++it) {
        seq += *it;
      }

      // remove elements from node stack
      nodeStack.erase(nodeStack.begin() + seqPos.back(),
                      nodeStack.end());
      seqPos.pop_back();

      // add new node, which corresponds to the sequence
      nodeStack.push_back(Node(seq));

      break;
    }

    case YAML_MAPPING_START_EVENT:
      mapPos.push_back(nodeStack.size());
      break;

    case YAML_MAPPING_END_EVENT: {
      // create new mapping
      Mapping mapping;
      QList<Node>::const_iterator key = nodeStack.begin() + mapPos.back();
      QList<Node>::const_iterator value = nodeStack.begin() + mapPos.back() + 1;

      while (key != nodeStack.end() && value != nodeStack.end()) {
        Mapping::const_iterator it = mapping.constFind(*key);
        if (it != mapping.constEnd()) {
          G_WARNING("WARNING: trying to insert key which already exists:" << *key
                    << "\n         Overwriting old value:" << it.value()
                    << "\n                with new value:" << *value);
        }
        mapping.insert(*key, *value);
        key += 2; value += 2;
      }

      if (value == nodeStack.end()) {
        // problem: the number of elements was not even (no match (key, value))
        throw YamlException("there is a key that is not matched by a value");
      }

      // remove elements from node stack
      nodeStack.erase(nodeStack.begin() + mapPos.back(),
                      nodeStack.end());
      mapPos.pop_back();

      // add new node, which corresponds to the sequence
      nodeStack.push_back(Node(mapping));

      break;
    }



    case YAML_NO_EVENT:
      cout << "NO EVENT FINISH!!" << endl;
      break;


    default:
      ;
      //cout << "unknown event" << endl;
    }

    yaml_event_delete(&event);

  } while(!done);

  yaml_parser_delete(&parser);

  if (nodeStack.size() != 1) {
    throw YamlException("Error while parsing, more than one node left in the end");
  }

  return nodeStack[0];
}



QString errorMessage(const yaml_parser_t& parser) {
  QString msg;

  switch (parser.error) {

  case YAML_MEMORY_ERROR:
    msg.sprintf("Memory error: Not enough memory for parsing");
    break;

  case YAML_READER_ERROR:
    if (parser.problem_value != -1) {
      msg.sprintf("Reader error: %s: #%X at %d", parser.problem,
                  parser.problem_value, (int)parser.problem_offset);
    }
    else {
      msg.sprintf("Reader error: %s at %d", parser.problem,
                  (int)parser.problem_offset);
    }
    break;

  case YAML_SCANNER_ERROR:
    if (parser.context) {
      msg.sprintf("Scanner error: %s at line %d, column %d\n"
                  "%s at line %d, column %d", parser.context,
                  (int)parser.context_mark.line+1, (int)parser.context_mark.column+1,
                  parser.problem, (int)parser.problem_mark.line+1,
                  (int)parser.problem_mark.column+1);
    }
    else {
      msg.sprintf("Scanner error: %s at line %d, column %d",
                  parser.problem, (int)parser.problem_mark.line+1,
                  (int)parser.problem_mark.column+1);
    }
    break;

  case YAML_PARSER_ERROR:
    if (parser.context) {
      msg.sprintf("Parser error: %s at line %d, column %d\n"
                  "%s at line %d, column %d", parser.context,
                  (int)parser.context_mark.line+1, (int)parser.context_mark.column+1,
                  parser.problem, (int)parser.problem_mark.line+1,
                  (int)parser.problem_mark.column+1);
    }
    else {
      msg.sprintf("Parser error: %s at line %d, column %d",
                  parser.problem, (int)parser.problem_mark.line+1,
                  (int)parser.problem_mark.column+1);
    }
    break;

  default:
    /* Couldn't happen. */
    msg.sprintf("Internal error in yaml parsing");
    break;
  }

  return msg;
}


int write_handler(void *ext, yaml_char_t *buffer, size_t size) {
  QByteArray* out = (QByteArray*)ext;
  out->append((char*)buffer, size);

  return 1;
}


QString emitterErrorMessage(yaml_emitter_t* emitter, const char* location) {
  QString msg;

  switch (emitter->error) {
  case YAML_MEMORY_ERROR:
    msg.sprintf("Memory error: Not enough memory for emitting (%s)", location);
    break;

  case YAML_WRITER_ERROR:
    msg.sprintf("Writer error: %s (%s)", emitter->problem, location);
    break;

  case YAML_EMITTER_ERROR:
    msg.sprintf("Emitter error: %s (%s)", emitter->problem, location);
    break;

  default:
    /* Couldn't happen. */
    msg.sprintf("Internal error");
    break;
  }

  return msg;
}

void dumpObject(yaml_emitter_t* emitter, const yaml::Node& node) {
  yaml_event_t event;

  switch (node.type()) {

  case yaml::ScalarType: {
    QByteArray str = node.scalar().toUtf8();
    yaml_scalar_event_initialize(&event, 0, 0, (yaml_char_t*)str.data(), str.size(), 1, 1, YAML_ANY_SCALAR_STYLE);
    if (!yaml_emitter_emit(emitter, &event)) throw YamlException(emitterErrorMessage(emitter, "scalar"));
    break;
  }


  case yaml::SequenceType: {
    yaml_sequence_start_event_initialize(&event, 0, 0, 1, YAML_ANY_SEQUENCE_STYLE);
    if (!yaml_emitter_emit(emitter, &event)) throw YamlException(emitterErrorMessage(emitter, "seq start"));

    for (int i=0; i<node.sequence().size(); i++) {
      dumpObject(emitter, node[i]);
    }

    yaml_sequence_end_event_initialize(&event);
    if (!yaml_emitter_emit(emitter, &event)) throw YamlException(emitterErrorMessage(emitter, "seq end"));
    break;
  }


  case yaml::MappingType: {
    yaml_mapping_start_event_initialize(&event, 0, 0, 1, YAML_ANY_MAPPING_STYLE);
    if (!yaml_emitter_emit(emitter, &event)) throw YamlException(emitterErrorMessage(emitter, "mapping start"));

    yaml::Mapping m = node.mapping();
    yaml::Mapping::const_iterator it;
    for (it = m.constBegin(); it != m.constEnd(); ++it) {
      dumpObject(emitter, it.key());
      dumpObject(emitter, it.value());
    }

    yaml_mapping_end_event_initialize(&event);
    if (!yaml_emitter_emit(emitter, &event)) throw YamlException(emitterErrorMessage(emitter, "mapping end"));
    break;
  }

  }

}

QByteArray dump(const yaml::Node& node) {

  yaml_emitter_t emitter;
  yaml_event_t event;

  yaml_emitter_initialize(&emitter);

  QByteArray result;
  yaml_emitter_set_output(&emitter, write_handler, &result);


  try {
    yaml_stream_start_event_initialize(&event, YAML_UTF8_ENCODING);
    if (!yaml_emitter_emit(&emitter, &event)) throw YamlException(emitterErrorMessage(&emitter, "stream start"));

    yaml_document_start_event_initialize(&event, 0, 0, 0, 0);
    if (!yaml_emitter_emit(&emitter, &event)) throw YamlException(emitterErrorMessage(&emitter, "document start"));

    dumpObject(&emitter, node);

    yaml_document_end_event_initialize(&event, 0);
    if (!yaml_emitter_emit(&emitter, &event)) throw YamlException(emitterErrorMessage(&emitter, "document end"));

    yaml_stream_end_event_initialize(&event);
    if (!yaml_emitter_emit(&emitter, &event)) throw YamlException(emitterErrorMessage(&emitter, "stream end"));

    yaml_emitter_delete(&emitter);

  }
  catch (YamlException&) {
    yaml_emitter_delete(&emitter);
    throw;
  }

  return result;
}


} // namespace yaml
} // namespace gaia2
