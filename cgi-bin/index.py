#!/usr/bin/python

import sys, wiringpi2 as wiringpi, json, subprocess, cgi

response_id = 1
response_value = ''
response_succeed = 1
response_toast = ''
response_debug = ''

#recuperation du body du POST (on recoit qqchose comme FieldStorage(None, None, '{objet json}) dans la variable body)
body = str(cgi.FieldStorage())
response_debug += 'Body du POST : ' + body + '. '

#recuperation de la string json (on enleve FieldStorage(None, None, ainsi que la derniere parenthese)
commande_json_string = body[26:(len(body)-2)]
response_debug += 'Commande json : ' + commande_json_string + '. '

#transformation en objet pour le parser
try:
  commande_json_objet = json.loads(commande_json_string)
  response_debug += 'Objet json : ' + str(commande_json_objet) + '. '
except:
  response_succeed = 0
  response_debug += 'Objet json : objet json non valide.'

try:
  if(commande_json_objet['id'] == 1):
    number = int(commande_json_objet['value'])
    if ((number == 0) | (number == 1)):
      proc = subprocess.Popen(["sudo", "python", "./gestion_hardware.py", str(number)], stdout=subprocess.PIPE)
      retour_hard = proc.communicate()[0]
      response_value = retour_hard[(len(retour_hard)-2):(len(retour_hard)-1)]
      response_toast = retour_hard
      response_debug += 'Traitement du json : Ordres transmis au hardware. '
      response_debug += 'Retour du pgm gestion_hardware : '+retour_hard + '. '
    else:
      response_succeed = 0
      response_debug += 'Traitement du json : Ordres incoherents (pb value). '
  else:
    response_succeed = 0
    response_debug += 'Traitement du json : Aucun equipement commande. '
except:
  response_succeed = 0
  response_debug += "Traitement du json : objet json sans id valide."

print 'Content-Type: application/json\n\n'
print json.dumps({'id' : response_id, 'value' : response_value, 'succeed': response_succeed,'toast': response_toast, 'debug': response_debug})

