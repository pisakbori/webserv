/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpError.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bpisak-l <bpisak-l@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/19 10:56:37 by bpisak-l          #+#    #+#             */
/*   Updated: 2024/11/13 15:50:33 by bpisak-l         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpError.hpp"

HttpError::HttpError() : _message("Undefined Error"), _code(200) {};

HttpError::HttpError(const std::string &msg, const int code) : _message(msg), _code(code) {};

HttpError::~HttpError() noexcept {};

HttpError::HttpError(HttpError const &ex) { *this = ex; };

HttpError &HttpError::operator=(const HttpError &ex)
{
	_message = ex.getMessage();
	_code = ex.code();
	return *this;
};

std::string const &HttpError::getMessage() const { return _message; };
int HttpError::code() const { return _code; };

const char *HttpError::what() const noexcept
{
	return _message.c_str();
}